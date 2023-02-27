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

let reduce_walk_result =
  object
    inherit
      [H.inter_constraint_ decorated WalkResult.t] Tast_visitor.reduce as super

    method zero = WalkResult.empty

    method plus = WalkResult.( @ )

    method! on_class_ env (A.{ c_name = (_, sid); c_extends; _ } as class_) =
      let id = H.Class sid in

      let at_extends acc (_, hint_) =
        match hint_ with
        | A.Happly ((hack_pos, parent_sid), _hints) ->
          let parent_id = H.Class parent_sid in
          let constraint_ = H.ClassExtends parent_id in
          let decorated =
            { origin = __LINE__; hack_pos; decorated_data = constraint_ }
          in
          WalkResult.add_constraint acc id decorated
        | _ -> acc
      in
      WalkResult.(
        let wr = List.fold ~init:WalkResult.empty ~f:at_extends c_extends in
        let wr = WalkResult.add_id wr id in
        wr @ super#on_class_ env class_)

    method! on_fun_def env (A.{ fd_name = (_, sid); _ } as fd) =
      let id = H.Function sid in
      WalkResult.add_id (super#on_fun_def env fd) id
  end

let program (ctx : Provider_context.t) (tast : Tast.program) :
    H.inter_constraint_ decorated WalkResult.t =
  let def acc def =
    let tast_env = Tast_env.def_env ctx def in
    WalkResult.(acc @ reduce_walk_result#on_def tast_env def)
  in
  List.fold ~init:WalkResult.empty ~f:def tast