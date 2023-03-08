(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Hh_prelude
open Sdt_analysis_types
module A = Aast
module T = Typing_defs
module SN = Naming_special_names
module MakeType = Typing_make_type
module Reason = Typing_reason
module TPEnv = Type_parameter_env

let mixed = MakeType.mixed Reason.Rnone

let supportdyn_of_mixed = MakeType.supportdyn Reason.Rnone mixed

let remove_supportdyn_of_mixed_upper_bound_from_tparams env =
  let typing_env = Tast_env.tast_env_as_typing_env env in
  let tpenv = typing_env.Typing_env_types.tpenv in
  let remove_upper_bound tpname tpinfo tpenv =
    let open Typing_kinding_defs in
    let upper_bounds = TySet.remove supportdyn_of_mixed tpinfo.upper_bounds in
    let tpinfo = { tpinfo with upper_bounds } in
    TPEnv.add ~def_pos:Pos_or_decl.none tpname tpinfo tpenv
  in
  let tpenv = TPEnv.fold remove_upper_bound tpenv TPEnv.empty in
  let typing_env = Typing_env_types.{ typing_env with tpenv } in
  Tast_env.typing_env_as_tast_env typing_env

let remove_supportdyn_from_ty ty =
  match T.get_node ty with
  | T.Tnewtype (c, [ty], _) when String.equal c SN.Classes.cSupportDyn -> ty
  | _ -> ty

let signature_check decorated_constraint env id params ret =
  let is_supportdyn_of_mixed ty =
    Tast_env.is_sub_type env ty supportdyn_of_mixed
  in
  let fails_formation =
    List.exists params ~f:(fun param ->
        not @@ is_supportdyn_of_mixed @@ fst param.A.param_type_hint)
    || (not @@ is_supportdyn_of_mixed @@ fst ret)
  in
  if fails_formation then
    let constraint_ = Lazy.force decorated_constraint in
    WalkResult.singleton id constraint_
  else
    WalkResult.empty

let collect_sdts =
  object
    inherit [Constraint.t decorated WalkResult.t] Tast_visitor.reduce as super

    method zero = WalkResult.empty

    method plus = WalkResult.( @ )

    method! on_expr env ((_, e_pos, e_) as e) =
      let decorated_constraint ~origin =
        { origin; hack_pos = e_pos; decorated_data = Constraint.NeedsSDT }
      in
      let walk_result =
        match e_ with
        | A.Call ((base_ty, _, base_exp), _tel, el, _unpacked_el) ->
          let doesnt_subtype (fp, (_, (arg_ty, _, _))) =
            not @@ Tast_env.is_sub_type env arg_ty fp.T.fp_type.T.et_type
          in
          let constraints_of_id id =
            let ty = remove_supportdyn_from_ty base_ty in
            match T.get_node ty with
            | T.Tfun ft ->
              let param_arg_pairs =
                let open List.Or_unequal_lengths in
                match List.zip ft.T.ft_params el with
                | Ok pairs -> pairs
                | Unequal_lengths -> []
              in
              if List.exists ~f:doesnt_subtype param_arg_pairs then
                let constraint_ = decorated_constraint ~origin:__LINE__ in
                WalkResult.singleton id constraint_
              else
                WalkResult.empty
            | _ -> WalkResult.empty
          in
          begin
            match base_exp with
            | A.Id (_, sid) -> constraints_of_id (H.Id.Function sid)
            | A.Obj_get ((receiver_ty, _, _), _, _, _) -> begin
              match T.get_node receiver_ty with
              | T.Tclass ((_, sid), _, _) ->
                constraints_of_id (H.Id.ClassLike sid)
              | _ -> WalkResult.empty
            end
            | A.Class_const ((_, _, A.CI (_, sid)), _) ->
              constraints_of_id (H.Id.ClassLike sid)
            | _ -> WalkResult.empty
          end
        | _ -> WalkResult.empty
      in
      WalkResult.(walk_result @ super#on_expr env e)

    method! on_method_ env m =
      let decorated_constraint ~origin =
        let hack_pos = m.A.m_span in
        { origin; hack_pos; decorated_data = Constraint.NeedsSDT }
      in
      let sid = Tast_env.get_self_id env |> Option.value_exn in
      let id = H.Id.ClassLike sid in
      let env = remove_supportdyn_of_mixed_upper_bound_from_tparams env in
      let from_signature_check =
        signature_check
          (lazy (decorated_constraint ~origin:__LINE__))
          env
          id
          m.A.m_params
          m.A.m_ret
      in
      WalkResult.(from_signature_check @ super#on_method_ env m)

    method! on_fun_def env (A.{ fd_name = (_, sid); fd_fun; _ } as fd) =
      let decorated_constraint ~origin =
        let hack_pos = fd_fun.A.f_span in
        { origin; hack_pos; decorated_data = Constraint.NeedsSDT }
      in
      let id = H.Id.Function sid in
      let from_dynamically_callable =
        let is_dynamically_callable =
          List.exists ~f:(fun attr ->
              String.equal
                (snd attr.A.ua_name)
                SN.UserAttributes.uaDynamicallyCallable)
        in
        if is_dynamically_callable fd_fun.A.f_user_attributes then
          let constraint_ = decorated_constraint ~origin:__LINE__ in
          WalkResult.singleton id constraint_
        else
          WalkResult.empty
      in
      let env = remove_supportdyn_of_mixed_upper_bound_from_tparams env in
      let from_signature_check =
        signature_check
          (lazy (decorated_constraint ~origin:__LINE__))
          env
          id
          fd_fun.A.f_params
          fd_fun.A.f_ret
      in
      WalkResult.(
        from_dynamically_callable
        @ from_signature_check
        @ super#on_fun_def env fd)
  end

let program (ctx : Provider_context.t) (tast : Tast.program) =
  let def acc def =
    let tast_env = Tast_env.def_env ctx def in
    WalkResult.(acc @ collect_sdts#on_def tast_env def)
  in
  List.fold ~init:WalkResult.empty ~f:def tast
