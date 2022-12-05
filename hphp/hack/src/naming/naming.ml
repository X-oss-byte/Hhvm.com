(*
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

(** Module "naming" a program.
 *
 * The naming phase consists in several things
 * 1- get all the global names
 * 2- transform all the local names into a unique identifier
 *)

open Hh_prelude
open Common
open String_utils
open Naming_phase_sigs
module N = Aast
module SN = Naming_special_names

(*****************************************************************************)
(* The types *)
(*****************************************************************************)

type genv = {
  (* strict? decl?  *)
  in_mode: FileInfo.mode;
  (* various options that control the strictness of the typechecker *)
  ctx: Provider_context.t;
  (* In function foo<T1, ..., Tn> or class<T1, ..., Tn>, the field
   * type_params knows T1 .. Tn. It is able to find out about the
   * constraint on these parameters. *)
  type_params: SSet.t;
  (* Namespace environment, e.g., what namespace we're in and what use
   * declarations are in play. *)
  namespace: Namespace_env.env;
}

(* The primitives to manipulate the naming environment *)
module Env : sig
  val make_class_env : Provider_context.t -> Nast.class_ -> genv

  val make_typedef_env : Provider_context.t -> Nast.typedef -> genv

  val make_top_level_env : Provider_context.t -> genv

  val make_fun_decl_genv : Provider_context.t -> Nast.fun_def -> genv

  val make_file_attributes_env :
    Provider_context.t -> FileInfo.mode -> Aast.nsenv -> genv

  val make_const_env : Provider_context.t -> Nast.gconst -> genv

  val make_module_env : Provider_context.t -> Nast.module_def -> genv
end = struct
  let get_tparam_names paraml =
    List.fold_right
      ~init:SSet.empty
      ~f:(fun { Aast.tp_name = (_, x); _ } acc -> SSet.add x acc)
      paraml

  let make_class_genv ctx tparams mode _ namespace _ =
    { in_mode = mode; ctx; type_params = get_tparam_names tparams; namespace }

  let make_class_env ctx c =
    let genv =
      make_class_genv
        ctx
        c.Aast.c_tparams
        c.Aast.c_mode
        (c.Aast.c_name, c.Aast.c_kind)
        c.Aast.c_namespace
        c.Aast.c_final
    in
    genv

  let make_typedef_genv ctx tparams tdef_namespace =
    {
      in_mode = FileInfo.Mstrict;
      ctx;
      type_params = get_tparam_names tparams;
      namespace = tdef_namespace;
    }

  let make_typedef_env ctx tdef =
    let genv =
      make_typedef_genv ctx tdef.Aast.t_tparams tdef.Aast.t_namespace
    in
    genv

  let make_fun_genv ctx params f_mode f_namespace =
    {
      in_mode = f_mode;
      ctx;
      type_params = get_tparam_names params;
      namespace = f_namespace;
    }

  let make_fun_decl_genv ctx f =
    make_fun_genv
      ctx
      f.Aast.fd_fun.Aast.f_tparams
      f.Aast.fd_mode
      f.Aast.fd_namespace

  let make_const_genv ctx cst =
    {
      in_mode = cst.Aast.cst_mode;
      ctx;
      type_params = SSet.empty;
      namespace = cst.Aast.cst_namespace;
    }

  let make_top_level_genv ctx =
    {
      in_mode = FileInfo.Mstrict;
      ctx;
      type_params = SSet.empty;
      namespace = Namespace_env.empty_with_default;
    }

  let make_top_level_env ctx =
    let genv = make_top_level_genv ctx in
    genv

  let make_file_attributes_genv ctx mode namespace =
    { in_mode = mode; ctx; type_params = SSet.empty; namespace }

  let make_file_attributes_env ctx mode namespace =
    let genv = make_file_attributes_genv ctx mode namespace in
    genv

  let make_const_env ctx cst =
    let genv = make_const_genv ctx cst in
    genv

  let make_module_genv ctx _module =
    {
      in_mode = FileInfo.Mstrict;
      ctx;
      type_params = SSet.empty;
      namespace = Namespace_env.empty_with_default;
    }

  let make_module_env ctx module_ =
    let genv = make_module_genv ctx module_ in
    genv
end

(*****************************************************************************)
(* Helpers *)
(*****************************************************************************)
let elaborate_namespaces =
  new Naming_elaborate_namespaces_endo.generic_elaborator

let check_repetition s param =
  let name = param.Aast.param_name in
  if SSet.mem name s then
    Errors.add_naming_error
    @@ Naming_error.Already_bound { pos = param.Aast.param_pos; name };
  if not (String.equal name SN.SpecialIdents.placeholder) then
    SSet.add name s
  else
    s

let invalid_expr_ (p : Pos.t) : Nast.expr_ =
  let throw : Nast.stmt =
    ( p,
      Aast.Throw
        ( (),
          p,
          Aast.New
            ( ((), p, Aast.CI (p, "\\Exception")),
              [],
              [((), p, Aast.String "invalid expression")],
              None,
              () ) ) )
  in
  Aast.Call
    ( ( (),
        p,
        Aast.Lfun
          ( {
              Aast.f_span = p;
              f_readonly_this = None;
              f_annotation = ();
              f_readonly_ret = None;
              f_ret = ((), None);
              f_name = (p, "invalid_expr");
              f_tparams = [];
              f_where_constraints = [];
              f_params = [];
              f_ctxs = None;
              f_unsafe_ctxs = None;
              f_body = { Aast.fb_ast = [throw] };
              f_fun_kind = Ast_defs.FSync;
              f_user_attributes = [];
              f_external = false;
              f_doc_comment = None;
            },
            [] ) ),
      [],
      [],
      None )

let ignored_expr_ p : Nast.expr_ = invalid_expr_ p

(**************************************************************************)
(* All the methods and static methods of an interface are "implicitly"
 * declared as abstract
 *)
(**************************************************************************)

let add_abstract m = { m with N.m_abstract = true }

let add_abstractl methods = List.map methods ~f:add_abstract

let interface c constructor methods smethods =
  if not (Ast_defs.is_c_interface c.Aast.c_kind) then
    (constructor, methods, smethods)
  else
    let constructor = Option.map constructor ~f:add_abstract in
    let methods = add_abstractl methods in
    let smethods = add_abstractl smethods in
    (constructor, methods, smethods)

let extend_tparams genv paraml =
  let params =
    List.fold_right
      paraml
      ~init:genv.type_params
      ~f:(fun { Aast.tp_name = (_, x); _ } acc -> SSet.add x acc)
  in
  { genv with type_params = params }

let make_xhp_attr = function
  | true -> Some { N.xai_like = None; N.xai_tag = None; N.xai_enum_values = [] }
  | false -> None

(**************************************************************************)
(* Statements *)
(**************************************************************************)

let rec stmt env (pos, st) =
  let stmt =
    match st with
    | Aast.Block _ -> failwith "stmt block error"
    | Aast.Fallthrough -> N.Fallthrough
    | Aast.Noop -> N.Noop
    | Aast.Markup _ -> N.Noop
    | Aast.AssertEnv _ -> N.Noop
    | Aast.Break -> Aast.Break
    | Aast.Continue -> Aast.Continue
    | Aast.Throw e -> N.Throw (expr env e)
    | Aast.Return e -> N.Return (Option.map e ~f:(expr env))
    | Aast.Yield_break -> N.Yield_break
    | Aast.Awaitall (el, b) -> awaitall_stmt env el b
    | Aast.If (e, b1, b2) -> if_stmt env e b1 b2
    | Aast.Do (b, e) -> do_stmt env b e
    | Aast.While (e, b) -> N.While (expr env e, block env b)
    | Aast.Using s ->
      using_stmt env s.Aast.us_has_await s.Aast.us_exprs s.Aast.us_block
    | Aast.For (st1, e, st2, b) -> for_stmt env st1 e st2 b
    | Aast.Switch (e, cl, dfl) -> switch_stmt env e cl dfl
    | Aast.Foreach (e, ae, b) -> foreach_stmt env e ae b
    | Aast.Try (b, cl, fb) -> try_stmt env b cl fb
    | Aast.Expr e -> N.Expr (expr env e)
  in
  (pos, stmt)

and awaitall_stmt env el b =
  let el =
    List.map
      ~f:(fun (e1, e2) ->
        let e2 = expr env e2 in
        (e1, e2))
      el
  in
  let s = block env b in
  N.Awaitall (el, s)

and if_stmt env e b1 b2 =
  let e = expr env e in
  let b1 = branch env b1 in
  let b2 = branch env b2 in
  N.If (e, b1, b2)

and do_stmt env b e =
  let b = block ~new_scope:false env b in
  let e = expr env e in
  N.Do (b, e)

(* Scoping is essentially that of do: block is always executed *)
and using_stmt env has_await (loc, e) b =
  let e = List.map ~f:(expr env) e in
  let b = block ~new_scope:false env b in
  N.Using
    N.
      {
        us_is_block_scoped = false;
        (* This isn't used for naming so provide a default *)
        us_has_await = has_await;
        us_exprs = (loc, e);
        us_block = b;
      }

and for_stmt env e1 e2 e3 b =
  let e1 = exprl env e1 in
  let e2 = oexpr env e2 in
  let b = block ~new_scope:false env b in
  let e3 = exprl env e3 in
  N.For (e1, e2, e3, b)

and switch_stmt env e cl dfl =
  let e = expr env e in
  let cl = casel env cl in
  let dfl = Option.map ~f:(fun (pos, b) -> (pos, branch env b)) dfl in
  N.Switch (e, cl, dfl)

and foreach_stmt env e ae b =
  let e = expr env e in
  let ae = as_expr env ae in
  let b = block env b in
  N.Foreach (e, ae, b)

and as_expr env ae =
  let handle_v ev =
    match ev with
    | (_, p, Aast.Id _) ->
      Errors.add_naming_error @@ Naming_error.Expected_variable p;
      let ident = Local_id.make_unscoped "__internal_placeholder" in
      ((), p, N.Lvar (p, ident))
    | ev -> expr env ev
  in
  let handle_k ek =
    match ek with
    | (_, _, Aast.Lvar (p, lid)) -> ((), p, N.Lvar (p, lid))
    | (_, p, _) ->
      Errors.add_naming_error @@ Naming_error.Expected_variable p;
      let ident = Local_id.make_unscoped "__internal_placeholder" in
      ((), p, N.Lvar (p, ident))
  in
  match ae with
  | Aast.As_v ev ->
    let ev = handle_v ev in
    N.As_v ev
  | Aast.As_kv (k, ev) ->
    let k = handle_k k in
    let ev = handle_v ev in
    N.As_kv (k, ev)
  | N.Await_as_v (p, ev) ->
    let ev = handle_v ev in
    N.Await_as_v (p, ev)
  | N.Await_as_kv (p, k, ev) ->
    let k = handle_k k in
    let ev = handle_v ev in
    N.Await_as_kv (p, k, ev)

and try_stmt env b cl fb =
  let fb = branch env fb in
  let b = branch env b in
  let cl = catchl env cl in
  N.Try (b, cl, fb)

and stmt_list stl env =
  match stl with
  | [] -> []
  | (_, Aast.Block b) :: rest ->
    let b = stmt_list b env in
    let rest = stmt_list rest env in
    b @ rest
  | x :: rest ->
    let x = stmt env x in
    let rest = stmt_list rest env in
    x :: rest

and block ?(new_scope = true) env stl =
  let _ = new_scope in
  stmt_list stl env

and branch env stmt_l = stmt_list stmt_l env

(**************************************************************************)
(* Expressions *)
(**************************************************************************)
and expr env ((), p, e) = ((), p, expr_ env p e)

and expr_ env p (e : Nast.expr_) =
  match e with
  | Aast.Varray (ta, l) -> N.Varray (ta, List.map l ~f:(expr env))
  | Aast.Darray (tap, l) ->
    N.Darray (tap, List.map l ~f:(fun (e1, e2) -> (expr env e1, expr env e2)))
  | Aast.ValCollection (kind, ta, exprs) ->
    Aast.ValCollection (kind, ta, List.map exprs ~f:(expr env))
  | Aast.KeyValCollection (kind, ta, fields) ->
    Aast.KeyValCollection
      ( kind,
        ta,
        List.map fields ~f:(fun (expr_key, expr_val) ->
            (expr env expr_key, expr env expr_val)) )
  | Aast.Clone e -> N.Clone (expr env e)
  | Aast.Null -> N.Null
  | Aast.True -> N.True
  | Aast.False -> N.False
  | Aast.Int s -> N.Int s
  | Aast.Float s -> N.Float s
  | Aast.String s -> N.String s
  | Aast.String2 idl -> N.String2 (string2 env idl)
  | Aast.PrefixedString (n, e) -> N.PrefixedString (n, expr env e)
  | Aast.Id x -> N.Id x
  | Aast.Lvar (_, x)
    when String.equal (Local_id.to_string x) SN.SpecialIdents.this ->
    N.This
  | Aast.Lvar (p, x)
    when String.equal (Local_id.to_string x) SN.SpecialIdents.dollardollar ->
    N.Dollardollar (p, Local_id.make_unscoped SN.SpecialIdents.dollardollar)
  | Aast.Lvar (p, x)
    when String.equal (Local_id.to_string x) SN.SpecialIdents.placeholder ->
    N.Lplaceholder p
  | Aast.Lvar x -> N.Lvar x
  | Aast.Obj_get (e1, e2, nullsafe, prop_or_method) ->
    (* If we encounter Obj_get(_,_,true) by itself, then it means "?->"
       is being used for instance property access; see the case below for
       handling nullsafe instance method calls to see how this works *)
    N.Obj_get (expr env e1, expr_obj_get_name env e2, nullsafe, prop_or_method)
  | Aast.Array_get (e1, e2) -> N.Array_get (expr env e1, oexpr env e2)
  | Aast.(Class_get (class_id, cg_expr, prop_or_method)) ->
    N.Class_get (class_id, cg_expr, prop_or_method)
  | Aast.Class_const (class_id, pstring) -> N.Class_const (class_id, pstring)
  | Aast.Tuple el -> N.Tuple (exprl env el)
  | Aast.Call (e, tal, el, unpacked_element) ->
    N.Call (expr env e, tal, expr_call_args env el, oexpr env unpacked_element)
  | Aast.FunctionPointer (fid, targs) -> N.FunctionPointer (fid, targs)
  | Aast.Yield e -> N.Yield (afield env e)
  | Aast.Await e -> N.Await (expr env e)
  | Aast.List el -> N.List (exprl env el)
  | Aast.Cast (ty, e2) -> N.Cast (ty, expr env e2)
  | Aast.ExpressionTree et ->
    N.ExpressionTree
      N.
        {
          et_hint = et.et_hint;
          et_splices = block env et.et_splices;
          et_function_pointers = block env et.et_function_pointers;
          et_virtualized_expr = expr env et.et_virtualized_expr;
          et_runtime_expr = expr env et.et_runtime_expr;
          et_dollardollar_pos = et.et_dollardollar_pos;
        }
  | Aast.ET_Splice e -> N.ET_Splice (expr env e)
  | Aast.Unop (uop, e) -> N.Unop (uop, expr env e)
  | Aast.Binop ((Ast_defs.Eq _ as bop), e1, e2) ->
    N.Binop (bop, expr env e1, expr env e2)
  | Aast.Binop (bop, e1, e2) -> N.Binop (bop, expr env e1, expr env e2)
  | Aast.Pipe (dollardollar, e1, e2) ->
    N.Pipe
      ( (fst dollardollar, Local_id.make_unscoped SN.SpecialIdents.dollardollar),
        expr env e1,
        expr env e2 )
  | Aast.Eif (e1, e2opt, e3) ->
    (* The order matters here, of course -- e1 can define vars that need to
     * be available in e2 and e3. *)
    let e1 = expr env e1 in
    let (e2opt, e3) =
      let e2opt = oexpr env e2opt in
      let e3 = expr env e3 in
      (e2opt, e3)
    in
    N.Eif (e1, e2opt, e3)
  | Aast.Is (e, h) -> N.Is (expr env e, h)
  | Aast.As (e, h, b) -> N.As (expr env e, h, b)
  | Aast.Upcast (e, h) -> N.Upcast (expr env e, h)
  | Aast.New (class_id, tal, el, unpacked_element, _) ->
    N.New (class_id, tal, exprl env el, oexpr env unpacked_element, ())
  | Aast.Efun (f, idl) ->
    let f = expr_lambda env f in
    N.Efun (f, idl)
  | Aast.Lfun (f, idl) ->
    let f = expr_lambda env f in
    N.Lfun (f, idl)
  | Aast.Xml (x, al, el) -> N.Xml (x, attrl env al, exprl env el)
  | Aast.Shape fdl ->
    let shp = List.map fdl ~f:(fun (pname, value) -> (pname, expr env value)) in
    N.Shape shp
  | Aast.Import _ -> ignored_expr_ p
  | Aast.Omitted -> N.Omitted
  | Aast.EnumClassLabel (opt_sid, x) -> N.EnumClassLabel (opt_sid, x)
  | Aast.ReadonlyExpr e -> N.ReadonlyExpr (expr env e)
  | Aast.Pair (targs_opt, e1, e2) -> N.Pair (targs_opt, expr env e1, expr env e2)
  | Aast.Fun_id nm -> N.Fun_id nm
  | Aast.Method_id (instance, meth) -> N.Method_id (expr env instance, meth)
  | Aast.Method_caller (c, meth) -> N.Method_caller (c, meth)
  | Aast.Smethod_id (cid, pstring) -> N.Smethod_id (cid, pstring)
  (* The below were not found on the AST.ml so they are not implemented here *)
  | Aast.Collection _
  | Aast.This
  | Aast.Dollardollar _
  | Aast.Lplaceholder _
  | Aast.Hole _ ->
    Errors.internal_error
      p
      "Malformed expr: Expr not found on legacy AST: T39599317";
    invalid_expr_ p

and expr_obj_get_name env expr_ =
  match expr_ with
  | (_, p, Aast.Id x) -> ((), p, N.Id x)
  | (_, p, e) -> expr env ((), p, e)

and exprl env l = List.map ~f:(expr env) l

and expr_call_args env = List.map ~f:(fun (pk, e) -> (pk, expr env e))

and oexpr env e = Option.map e ~f:(expr env)

and expr_lambda env f =
  let f_params = fun_paraml env f.Aast.f_params in
  (* The bodies of lambdas go through naming in the containing local
   * environment *)
  let fb_ast = f_body env f.Aast.f_body in
  (* These could all be probably be replaced with a {... where ...} *)
  let f_body = Aast.{ fb_ast } in
  let f_user_attributes = user_attributes env f.Aast.f_user_attributes in
  Aast.
    {
      f with
      f_annotation = ();
      f_params;
      f_body;
      f_tparams = [];
      f_where_constraints = [];
      f_user_attributes;
    }

and f_body env f_body = block env f_body.Aast.fb_ast

and casel env l = List.map l ~f:(case env)

and case env (e, b) =
  let e = expr env e in
  let b = branch env b in
  (e, b)

and catchl env l = List.map l ~f:(catch env)

and catch env ((p1, lid1), (p2, lid2), b) =
  let b = branch env b in
  ((p1, lid1), (p2, lid2), b)

and afield env field =
  match field with
  | Aast.AFvalue e -> N.AFvalue (expr env e)
  | Aast.AFkvalue (e1, e2) -> N.AFkvalue (expr env e1, expr env e2)

and attr env at =
  match at with
  | Aast.Xhp_simple { Aast.xs_name; xs_type; xs_expr = e } ->
    N.Xhp_simple { Aast.xs_name; xs_type; xs_expr = expr env e }
  | Aast.Xhp_spread e -> N.Xhp_spread (expr env e)

and attrl env l = List.map ~f:(attr env) l

and string2 env idl = List.map idl ~f:(expr env)

(**************************************************************************)
(* Functions *)
(**************************************************************************)
and fun_ genv f =
  let env = genv in
  let f_params = fun_paraml env f.Aast.f_params in
  let f_tparams = type_paraml env f.Aast.f_tparams in
  (* TODO[mjt] pull out into elaboration pass *)
  let f_body =
    match genv.in_mode with
    | FileInfo.Mhhi -> { N.fb_ast = [] }
    | FileInfo.Mstrict ->
      let fb_ast = block env f.Aast.f_body.Aast.fb_ast in
      { N.fb_ast }
  in
  let f_user_attributes = user_attributes env f.Aast.f_user_attributes in
  Aast.
    { f with f_annotation = (); f_tparams; f_params; f_body; f_user_attributes }

(* Variadic params are removed from the list *)
and fun_param env (param : Nast.fun_param) =
  let param_expr = Option.map param.Aast.param_expr ~f:(expr env) in
  let param_user_attributes =
    user_attributes env param.Aast.param_user_attributes
  in
  Aast.{ param with param_annotation = (); param_expr; param_user_attributes }

and fun_paraml env paraml =
  let _ = List.fold_left ~f:check_repetition ~init:SSet.empty paraml in
  List.map ~f:(fun_param env) paraml

(**************************************************************************)
(* User attrs *)
(**************************************************************************)
and user_attributes env attrl =
  let on_attr acc { Aast.ua_name; ua_params } =
    let attr = { N.ua_name; N.ua_params = List.map ~f:(expr env) ua_params } in
    attr :: acc
  in
  List.fold_left ~init:[] ~f:on_attr attrl

(**************************************************************************)
(* Type parameters *)
(**************************************************************************)
(*
  We need to be careful regarding the scoping of type variables:
  Type parameters are always in scope simultaneously: Given
  class C<T1 ... , T2 ... , Tn ...>,
  all type parameters are in scope in the constraints of all other ones (and the where constraints,
  in case of functions).
  For consitency, the same holds for nested type parameters (i.e., type parameters of type
  parameters). Given
  class Foo<T<T1 ... , ...., Tn ... > ... >
  every Ti is in scope of the constraints of all other Tj, and in the constraints on T itself.
*)
and type_param ~forbid_this genv t =
  let hk_types_enabled =
    TypecheckerOptions.higher_kinded_types (Provider_context.get_tcopt genv.ctx)
  in
  let (pos, name) = t.Aast.tp_name in

  if (not hk_types_enabled) && (not @@ List.is_empty t.Aast.tp_parameters) then
    Errors.add_naming_error
    @@ Naming_error.Tparam_with_tparam { pos; tparam_name = name };

  (* Bring all type parameters into scope at once before traversing nested tparams,
     as per the note above *)
  let env = extend_tparams genv t.Aast.tp_parameters in
  let tp_parameters =
    if hk_types_enabled then
      List.map t.Aast.tp_parameters ~f:(type_param ~forbid_this env)
    else
      []
  in
  (* Use the env with all nested tparams still in scope *)
  let tp_constraints = t.Aast.tp_constraints in
  {
    N.tp_variance = t.Aast.tp_variance;
    tp_name = t.Aast.tp_name;
    tp_parameters;
    tp_constraints;
    tp_reified = t.Aast.tp_reified;
    tp_user_attributes = user_attributes env t.Aast.tp_user_attributes;
  }

and type_paraml ?(forbid_this = false) env tparams =
  List.map tparams ~f:(type_param ~forbid_this env)

(**************************************************************************)
(* Type constants *)
(**************************************************************************)

let typeconst env t =
  let open Aast in
  let tconst = t.c_tconst_kind in
  let attrs = user_attributes env t.Aast.c_tconst_user_attributes in
  N.
    {
      c_tconst_user_attributes = attrs;
      c_tconst_name = t.Aast.c_tconst_name;
      c_tconst_kind = tconst;
      c_tconst_span = t.Aast.c_tconst_span;
      c_tconst_doc_comment = t.Aast.c_tconst_doc_comment;
      c_tconst_is_ctx = t.Aast.c_tconst_is_ctx;
    }

(**************************************************************************)
(* Methods *)
(**************************************************************************)

let method_ genv m =
  let genv = extend_tparams genv m.Aast.m_tparams in
  let env = genv in
  (* Cannot use 'this' if it is a public instance method *)
  let m_params = fun_paraml env m.Aast.m_params in
  let m_tparams = type_paraml env m.Aast.m_tparams in
  let m_body =
    match genv.in_mode with
    | FileInfo.Mhhi -> { N.fb_ast = [] }
    | FileInfo.Mstrict ->
      let fub_ast = block env m.N.m_body.N.fb_ast in
      { N.fb_ast = fub_ast }
  in
  let m_user_attributes = user_attributes env m.Aast.m_user_attributes in
  Aast.
    { m with m_annotation = (); m_tparams; m_params; m_body; m_user_attributes }

(**************************************************************************)
(* Top level function definitions *)
(**************************************************************************)

let file_attribute ctx mode fa =
  let env = Env.make_file_attributes_env ctx mode fa.Aast.fa_namespace in
  let ua = user_attributes env fa.Aast.fa_user_attributes in
  N.{ fa_user_attributes = ua; fa_namespace = fa.Aast.fa_namespace }

let file_attributes ctx mode fal = List.map ~f:(file_attribute ctx mode) fal

let fun_def_help ctx genv fd =
  (* TODO[mjt] pull out into a tcopt elaboration pass *)
  let fd =
    if
      Provider_context.get_tcopt ctx |> TypecheckerOptions.substitution_mutation
      && FileInfo.equal_mode fd.Aast.fd_mode FileInfo.Mstrict
    then
      Substitution_mutation.mutate_fun_def fd
    else
      fd
  in
  (* TODO[mjt] pull out into an elaboration pass *)
  let fd = Naming_captures.populate_fun_def fd in

  let fd_file_attributes =
    file_attributes ctx fd.Aast.fd_mode fd.Aast.fd_file_attributes
  in
  let fd_fun = fun_ genv fd.Aast.fd_fun in
  Aast.{ fd with fd_fun; fd_file_attributes }

(**************************************************************************)
(* Classes *)
(**************************************************************************)

let class_prop_expr_is_xhp env cv =
  let expr = Option.map cv.Aast.cv_expr ~f:(expr env) in
  let expr =
    if FileInfo.is_hhi env.in_mode && Option.is_none expr then
      let pos = fst cv.Aast.cv_id in
      Some ((), pos, ignored_expr_ (fst cv.Aast.cv_id))
    else
      expr
  in
  let is_xhp =
    try String.(sub (snd cv.Aast.cv_id) ~pos:0 ~len:1 = ":") with
    | Invalid_argument _ -> false
  in
  (expr, is_xhp)

let class_prop_static env cv =
  let cv_user_attributes = user_attributes env cv.Aast.cv_user_attributes in
  let (cv_expr, is_xhp) = class_prop_expr_is_xhp env cv in
  let cv_xhp_attr = make_xhp_attr is_xhp in
  Aast.{ cv with cv_xhp_attr; cv_expr; cv_user_attributes }

let class_prop_non_static env ?(const = None) cv =
  (* if class is __Const, make all member fields __Const *)
  let cv_user_attributes =
    let attrs = user_attributes env cv.Aast.cv_user_attributes in
    match const with
    | Some c ->
      if not (Naming_attributes.mem SN.UserAttributes.uaConst attrs) then
        c :: attrs
      else
        attrs
    | None -> attrs
  in
  let (cv_expr, is_xhp) = class_prop_expr_is_xhp env cv in
  let cv_xhp_attr = make_xhp_attr is_xhp in
  Aast.{ cv with cv_xhp_attr; cv_expr; cv_user_attributes }

let class_const env ~in_enum_class:_ cc =
  Aast.
    {
      cc with
      cc_user_attributes = user_attributes env cc.Aast.cc_user_attributes;
    }

(* h cv is_required maybe_enum *)
let xhp_attribute_decl env (h, cv, tag, maybe_enum) =
  let (p, id) = cv.Aast.cv_id in
  let default = cv.Aast.cv_expr in
  let is_required = Option.is_some tag in
  if is_required && Option.is_some default then
    Errors.add_naming_error
    @@ Naming_error.Xhp_required_with_default { pos = p; attr_name = id };
  let hint_ =
    match maybe_enum with
    | Some (pos, items) ->
      let is_int item =
        match item with
        | (_, _, Aast.Int _) -> true
        | _ -> false
      in
      let contains_int = List.exists ~f:is_int items in
      let is_string item =
        match item with
        | (_, _, Aast.String _)
        | (_, _, Aast.String2 _) ->
          true
        | _ -> false
      in
      let contains_str = List.exists ~f:is_string items in
      if contains_int && not contains_str then
        Some (pos, Aast.(Hprim Tint))
      else if (not contains_int) && contains_str then
        Some (pos, Aast.(Hprim Tstring))
      else
        Some (pos, Aast.Hmixed)
    | _ -> Aast.hint_of_type_hint h
  in
  let strip_like h =
    match h with
    | Aast.Hlike h -> snd h
    | _ -> h
  in
  let hint_ =
    match hint_ with
    | Some (p, h) ->
      begin
        match strip_like h with
        | Aast.Hoption _ ->
          if is_required then
            Errors.add_naming_error
            @@ Naming_error.Xhp_optional_required_attr
                 { pos = p; attr_name = id };
          hint_
        | Aast.Hmixed -> hint_
        | _ ->
          let has_default =
            match default with
            | None
            | Some (_, _, Aast.Null) ->
              false
            | _ -> true
          in
          if is_required || has_default then
            hint_
          else
            Some (p, Aast.Hoption (p, h))
      end
    | None -> None
  in
  let (like, enum_values) =
    match cv.Aast.cv_xhp_attr with
    | Some xai -> (xai.Aast.xai_like, xai.Aast.xai_enum_values)
    | None -> (None, [])
  in
  let hint_ =
    Option.map
      ~f:(fun hint ->
        match like with
        | Some plike ->
          if
            not
              (TypecheckerOptions.like_type_hints
                 (Provider_context.get_tcopt env.ctx))
          then
            Errors.experimental_feature p "like-types";

          (plike, Aast.Hlike hint)
        | _ -> hint)
      hint_
  in
  let cv_type = ((), hint_) in
  let (cv_expr, _) = class_prop_expr_is_xhp env cv in
  let cv_xhp_attr =
    Some { N.xai_like = like; N.xai_tag = tag; N.xai_enum_values = enum_values }
  in
  Aast.{ cv with cv_xhp_attr; cv_type; cv_expr; cv_user_attributes = [] }

let class_help ctx env c =
  let c = Naming_captures.populate_class_ c in
  let (constructor, smethods, methods) = Aast.split_methods c.Aast.c_methods in
  let smethods = List.map ~f:(method_ env) smethods in
  let (sprops, props) = Aast.split_vars c.Aast.c_vars in
  let sprops = List.map ~f:(class_prop_static env) sprops in
  (* The attributes applied to a class exist outside the current class so references to `self` are invalid *)
  let attrs = user_attributes env c.Aast.c_user_attributes in
  let const = Naming_attributes.find SN.UserAttributes.uaConst attrs in
  let props = List.map ~f:(class_prop_non_static ~const env) props in
  let xhp_attrs = List.map ~f:(xhp_attribute_decl env) c.Aast.c_xhp_attrs in
  (* These would be out of order with the old attributes, but that shouldn't matter? *)
  let props = props @ xhp_attrs in
  let in_enum_class =
    let open Ast_defs in
    match c.Aast.c_kind with
    | Cenum_class _ -> true
    | Cclass _
    | Cinterface
    | Cenum
    | Ctrait ->
      false
  in
  let methods = List.map ~f:(method_ env) methods in
  let uses = c.Aast.c_uses in
  let xhp_attr_uses = c.Aast.c_xhp_attr_uses in
  let (c_req_extends, c_req_implements, c_req_class) =
    Aast.split_reqs c.Aast.c_reqs
  in
  (* TODO[mjt] pull out into a validation pass *)
  if
    (not (List.is_empty c_req_implements))
    && not (Ast_defs.is_c_trait c.Aast.c_kind)
  then
    Errors.add_naming_error
    @@ Naming_error.Invalid_require_implements
         (fst (List.hd_exn c_req_implements));
  if
    (not (List.is_empty c_req_class)) && not (Ast_defs.is_c_trait c.Aast.c_kind)
  then
    Errors.add_naming_error
    @@ Naming_error.Invalid_require_class (fst (List.hd_exn c_req_class));

  let e =
    TypecheckerOptions.explicit_consistent_constructors
      (Provider_context.get_tcopt ctx)
  in
  if e > 0 && Option.is_none constructor then
    Option.iter
      (Naming_attributes.mem_pos SN.UserAttributes.uaConsistentConstruct attrs)
      ~f:(fun pos ->
        let err_opt =
          match c.Aast.c_kind with
          | Ast_defs.Ctrait ->
            Some
              (Naming_error.Explicit_consistent_constructor
                 { classish_kind = c.Aast.c_kind; pos })
          | _ when e > 1 ->
            Some
              (Naming_error.Explicit_consistent_constructor
                 { classish_kind = c.Aast.c_kind; pos })
          | _ -> None
        in
        Option.iter err_opt ~f:Errors.add_naming_error);
  let req_implements = c_req_implements in
  let req_implements =
    List.map ~f:(fun h -> (h, N.RequireImplements)) req_implements
  in
  if
    (not (List.is_empty c_req_extends))
    && (not (Ast_defs.is_c_trait c.Aast.c_kind))
    && not (Ast_defs.is_c_interface c.Aast.c_kind)
  then
    Errors.add_naming_error
    @@ Naming_error.Invalid_require_extends (fst (List.hd_exn c_req_extends));
  let req_extends = c_req_extends in
  let req_extends = List.map ~f:(fun h -> (h, N.RequireExtends)) req_extends in
  let req_class = c_req_class in
  let req_class = List.map ~f:(fun h -> (h, N.RequireClass)) req_class in
  (* Setting a class type parameters constraint to the 'this' type is weird
   * so lets forbid it for now.
   *)
  let c_tparams = type_paraml env c.Aast.c_tparams in
  let consts = List.map ~f:(class_const env ~in_enum_class) c.Aast.c_consts in
  let typeconsts = List.map ~f:(typeconst env) c.Aast.c_typeconsts in
  let implements = c.Aast.c_implements in
  let constructor = Option.map constructor ~f:(method_ env) in
  let (constructor, methods, smethods) =
    interface c constructor methods smethods
  in
  let file_attributes =
    file_attributes ctx c.Aast.c_mode c.Aast.c_file_attributes
  in
  let methods =
    match constructor with
    | None -> smethods @ methods
    | Some c -> c :: smethods @ methods
  in
  Aast.
    {
      c with
      c_annotation = ();
      c_tparams;
      c_uses = uses;
      c_xhp_attr_uses = xhp_attr_uses;
      c_reqs = req_extends @ req_implements @ req_class;
      c_implements = implements;
      c_consts = consts;
      c_typeconsts = typeconsts;
      c_vars = sprops @ props;
      c_methods = methods;
      c_user_attributes = attrs;
      c_file_attributes = file_attributes;
      (* Naming and typechecking shouldn't use these fields *)
      c_xhp_attrs = [];
    }

(**************************************************************************)
(* Typedefs *)
(**************************************************************************)

let typedef_help ctx env tdef =
  let t_user_attributes = user_attributes env tdef.Aast.t_user_attributes in
  let t_file_attributes =
    file_attributes ctx tdef.Aast.t_mode tdef.Aast.t_file_attributes
  in
  let t_tparams = type_paraml env tdef.Aast.t_tparams in
  Aast.
    {
      tdef with
      t_tparams;
      t_user_attributes;
      t_file_attributes;
      t_annotation = ();
    }

(**************************************************************************)
(* Global constants *)
(**************************************************************************)

let global_const_help _ _env cst = Aast.{ cst with cst_annotation = () }

(**************************************************************************)
(* Module declarations *)
(**************************************************************************)

let module_help ctx env module_ =
  (* TODO[mjt]: pull this into tcopt validation pass *)
  let tcopts = Provider_context.get_tcopt ctx in
  let allowed_files =
    TypecheckerOptions.allowed_files_for_module_declarations tcopts
  in
  let allow_all_files =
    TypecheckerOptions.allow_all_files_for_module_declarations tcopts
  in
  let open Aast in
  let module_file = Relative_path.suffix @@ Pos.filename module_.md_span in
  if
    (not allow_all_files)
    && not
         (List.exists allowed_files ~f:(fun allowed_file ->
              let len = String.length allowed_file in
              if len > 0 then
                match allowed_file.[len - 1] with
                | '*' ->
                  let allowed_dir =
                    String.sub allowed_file ~pos:0 ~len:(len - 1)
                  in
                  String_utils.string_starts_with module_file allowed_dir
                | _ -> String.equal allowed_file module_file
              else
                false))
  then
    Errors.add_naming_error
    @@ Naming_error.Module_declaration_outside_allowed_files module_.md_span;
  let md_user_attributes = user_attributes env module_.md_user_attributes in
  { module_ with md_annotation = (); md_user_attributes }

(**************************************************************************)
(* Programs *)
(**************************************************************************)

let program_help ctx env ast =
  let top_level_env = ref env in
  let rec aux acc def =
    match def with
    | Aast.Fun f ->
      let genv = Env.make_fun_decl_genv ctx f in
      N.Fun (fun_def_help ctx genv f) :: acc
    | Aast.Class c ->
      let env = Env.make_class_env ctx c in
      N.Class (class_help ctx env c) :: acc
    | Aast.Stmt (_, Aast.Noop)
    | Aast.Stmt (_, Aast.Markup _) ->
      acc
    | Aast.Stmt s -> N.Stmt (stmt !top_level_env s) :: acc
    | Aast.Typedef t ->
      let env = Env.make_typedef_env ctx t in
      N.Typedef (typedef_help ctx env t) :: acc
    | Aast.Constant cst ->
      let env = Env.make_const_env ctx cst in
      N.Constant (global_const_help ctx env cst) :: acc
    | Aast.Namespace (_ns, aast) -> List.fold_left ~f:aux ~init:[] aast @ acc
    | Aast.NamespaceUse _ -> acc
    | Aast.SetNamespaceEnv nsenv ->
      let genv = !top_level_env in
      let genv = { genv with namespace = nsenv } in
      top_level_env := genv;
      acc
    | Aast.Module md ->
      let env = Env.make_module_env ctx md in
      N.Module (module_help ctx env md) :: acc
    | Aast.SetModule sm -> N.SetModule sm :: acc
    (* These are elaborated away in Namespaces.elaborate_toplevel_defs *)
    | Aast.FileAttributes _ -> acc
  in
  let on_program aast =
    let nast = List.fold_left ~f:aux ~init:[] aast in
    List.rev nast
  in
  on_program ast

(**************************************************************************)
(* The entry points to CHECK the program, and transform the program *)
(**************************************************************************)

type 'elem pipeline = {
  elab_ns: 'elem -> 'elem;
  elab_hints: (Naming_elab_hints.Env.t, 'elem) elabidation;
  elab_collection: (Naming_elab_collection.Env.t, 'elem) elabidation;
  elab_call: (Naming_elab_call.Env.t, 'elem) elabidation;
  elab_tuple: (Naming_elab_tuple.Env.t, 'elem) elabidation;
  elab_invariant: (Naming_elab_invariant.Env.t, 'elem) elabidation;
  elab_const_expr: (Naming_elab_const_expr.Env.t, 'elem) elabidation;
  elab_user_attrs: (Naming_elab_user_attributes.Env.t, 'elem) elabidation;
  elab_help: Provider_context.t -> genv -> 'elem -> 'elem;
  elab_soft: (Naming_elab_soft.Env.t, 'elem) elaboration;
  elab_everything_sdt: (Naming_elab_everything_sdt.Env.t, 'elem) elaboration;
  elab_hkt: (Naming_elab_hkt.Env.t, 'elem) elabidation;
  elab_enum_class: (Naming_elab_enum_class.Env.t, 'elem) elaboration;
  elab_class_id: (Naming_elab_class_id.Env.t, 'elem) elabidation;
  elab_dynamic_class_name:
    (Naming_elab_dynamic_class_name.Env.t, 'elem) elabidation;
  validate_xhp: (Naming_validate_xhp_name.Env.t, 'elem) validation;
  validate_builtin_enum: (Naming_validate_builtin_enum.Env.t, 'elem) validation;
  validate_enum_class_typeconst:
    (Naming_validate_enum_class_typeconst.Env.t, 'elem) validation;
  validate_supportdyn: (Naming_validate_supportdyn.Env.t, 'elem) validation;
}

(* Apply our elaboration and validation steps to a given ast element *)
let elab_elem
    elem
    ~ctx
    ~env
    ~filename
    {
      elab_ns;
      elab_hints;
      elab_collection;
      elab_call;
      elab_tuple;
      elab_invariant;
      elab_const_expr;
      elab_user_attrs;
      elab_help;
      elab_soft;
      elab_everything_sdt;
      elab_hkt;
      elab_enum_class;
      elab_class_id;
      elab_dynamic_class_name;
      validate_xhp;
      validate_builtin_enum;
      validate_enum_class_typeconst;
      validate_supportdyn;
    } =
  let tcopt = Provider_context.get_tcopt ctx in
  (* Elaborate namespaces *)
  let elem = elab_ns elem in

  (* Elaborate hints, collect errors and report them *)
  let (elem, err) = elab_hints elem in
  (* We have to check if like-types are globally enabled and remove errors
     before reporting if so *)
  let err =
    if TypecheckerOptions.like_type_hints tcopt then
      Naming_phase_error.suppress_like_type_errors err
    else
      err
  in
  (* Check for invalid use of internal classes - note that we must have this
     validation pass _before_ we elaborate enum classes *)
  let err =
    if
      not
        (string_ends_with (Relative_path.suffix filename) ".hhi"
        || TypecheckerOptions.is_systemlib (Provider_context.get_tcopt ctx))
    then
      validate_builtin_enum ~init:err elem
    else
      err
  in

  (* Add implicit extends for enums & enum classes *)
  let elem = elab_enum_class elem in

  (* Use canonical class_ids and validate usage of special names outside classes *)
  let (elem, err) = elab_class_id ~init:err elem in

  (* Warn and rewrite dynamic class_id expressions to unknown class ident
     NB: this must be sequenced after class_id elaboration so that class_id's
     are in canonical form, specifically that any remaining `CIexpr` represent
     dynamic new's
  *)
  let (elem, err) = elab_dynamic_class_name ~init:err elem in

  (* Elaboration `Collection` to `ValCollection` or `KeyValCollection *)
  let (elem, err) = elab_collection ~init:err elem in

  let (elem, err) = elab_call ~init:err elem in

  let (elem, err) = elab_tuple ~init:err elem in

  let (elem, err) = elab_invariant ~init:err elem in

  let (elem, err) = elab_const_expr ~init:err elem in

  let (elem, err) = elab_user_attrs ~init:err elem in

  (* General expression / statement / xhp elaboration & validation *)
  let elem = elab_help ctx env elem in

  (* Miscellaneous validation  *)
  (* TODO[mjt] move these to NAST checks*)
  (* Check for specific errors when referring to xhp classes *)
  let err = validate_xhp ~init:err elem in

  (* Apply elaboration / validation based on typechecker options *)
  (* Soft types *)
  let soft_as_like =
    TypecheckerOptions.interpret_soft_types_as_like_types tcopt
  in
  let elem = elab_soft elem ~env:soft_as_like in

  (* If HKTs are not enabled, we remove type parameter here and generate
     specific errors rather than arity errors in typing *)
  (* TODO[mjt] you do get an arity error from typing if you don't do
     this - we might consider specializing _that_ error to give info
     about HKTs rather than doing this from naming *)
  let hkt_enabled = TypecheckerOptions.higher_kinded_types tcopt in
  let (elem, err) =
    if not hkt_enabled then
      elab_hkt ~init:err elem
    else
      (elem, err)
  in

  (* SupportDyn *)
  let err =
    if
      (not
         (string_ends_with (Relative_path.suffix filename) ".hhi"
         || TypecheckerOptions.is_systemlib tcopt))
      && not
           (TypecheckerOptions.experimental_feature_enabled
              tcopt
              TypecheckerOptions.experimental_supportdynamic_type_hint)
    then
      validate_supportdyn ~init:err elem
    else
      err
  in

  (* Sound dynamic *)
  let elem =
    if TypecheckerOptions.everything_sdt tcopt then
      elab_everything_sdt elem
    else
      elem
  in

  (* enum class type constants *)
  (* we're still in the middle of developing type constants for enum classes
     so we gate them carefully for now:
     They must use the feature flag `type_constants_in_enum_class` AND
     be in a selected list of directories.

     For internal testing, we provide a global "enable" flag to just
     enable them. This is off by default except in hh_single_type_check.
  *)
  let class_dir = Filename.dirname @@ Relative_path.suffix filename in
  let err =
    if
      TypecheckerOptions.allow_all_locations_for_type_constant_in_enum_class
        tcopt
      || List.exists ~f:(String.equal class_dir)
         @@ TypecheckerOptions.allowed_locations_for_type_constant_in_enum_class
              tcopt
    then
      err
    else
      validate_enum_class_typeconst ~init:err elem
  in

  Naming_phase_error.emit err;
  elem

let program_filename defs =
  let open Aast_defs in
  let rec aux = function
    | Fun fun_def :: _ -> Pos.filename fun_def.fd_fun.f_span
    | Class class_ :: _ -> Pos.filename class_.c_span
    | Stmt (pos, _) :: _ -> Pos.filename pos
    | Typedef typedef :: _ -> Pos.filename typedef.t_span
    | Constant gconst :: _ -> Pos.filename gconst.cst_span
    | Module module_def :: _ -> Pos.filename module_def.md_span
    | _ :: rest -> aux rest
    | _ -> Relative_path.default
  in
  aux defs

let program ctx ast =
  let env = Env.make_top_level_env ctx in
  let filename = program_filename ast in
  elab_elem
    ast
    ~ctx
    ~env
    ~filename
    {
      elab_ns =
        elaborate_namespaces#on_program
          (Naming_elaborate_namespaces_endo.make_env
             Namespace_env.empty_with_default);
      elab_hints = Naming_elab_hints.elab_program;
      elab_collection = Naming_elab_collection.elab_program;
      elab_call = Naming_elab_call.elab_program;
      elab_tuple = Naming_elab_tuple.elab_program;
      elab_invariant = Naming_elab_invariant.elab_program;
      elab_const_expr = Naming_elab_const_expr.elab_program;
      elab_user_attrs = Naming_elab_user_attributes.elab_program;
      elab_help = program_help;
      elab_soft = Naming_elab_soft.elab_program;
      elab_everything_sdt = Naming_elab_everything_sdt.elab_program;
      elab_hkt = Naming_elab_hkt.elab_program;
      elab_enum_class = Naming_elab_enum_class.elab_program;
      elab_class_id = Naming_elab_class_id.elab_program;
      elab_dynamic_class_name = Naming_elab_dynamic_class_name.elab_program;
      validate_xhp = Naming_validate_xhp_name.validate_program;
      validate_builtin_enum = Naming_validate_builtin_enum.validate_program;
      validate_enum_class_typeconst =
        Naming_validate_enum_class_typeconst.validate_program;
      validate_supportdyn = Naming_validate_supportdyn.validate_program;
    }

let fun_def ctx fd =
  let env = Env.make_fun_decl_genv ctx fd in
  let filename = Pos.filename fd.Aast.fd_fun.Aast.f_span in
  elab_elem
    fd
    ~ctx
    ~env
    ~filename
    {
      elab_ns =
        elaborate_namespaces#on_fun_def
          (Naming_elaborate_namespaces_endo.make_env env.namespace);
      elab_hints = Naming_elab_hints.elab_fun_def;
      elab_collection = Naming_elab_collection.elab_fun_def;
      elab_call = Naming_elab_call.elab_fun_def;
      elab_tuple = Naming_elab_tuple.elab_fun_def;
      elab_invariant = Naming_elab_invariant.elab_fun_def;
      elab_const_expr = Naming_elab_const_expr.elab_fun_def;
      elab_user_attrs = Naming_elab_user_attributes.elab_fun_def;
      elab_help = fun_def_help;
      elab_soft = Naming_elab_soft.elab_fun_def;
      elab_everything_sdt = Naming_elab_everything_sdt.elab_fun_def;
      elab_hkt = Naming_elab_hkt.elab_fun_def;
      elab_enum_class = Naming_elab_enum_class.elab_fun_def;
      elab_class_id = Naming_elab_class_id.elab_fun_def;
      elab_dynamic_class_name = Naming_elab_dynamic_class_name.elab_fun_def;
      validate_xhp = Naming_validate_xhp_name.validate_fun_def;
      validate_builtin_enum = Naming_validate_builtin_enum.validate_fun_def;
      validate_enum_class_typeconst =
        Naming_validate_enum_class_typeconst.validate_fun_def;
      validate_supportdyn = Naming_validate_supportdyn.validate_fun_def;
    }

let class_ ctx c =
  let env = Env.make_class_env ctx c in
  let filename = Pos.filename c.Aast.c_span in
  elab_elem
    c
    ~ctx
    ~env
    ~filename
    {
      elab_ns =
        elaborate_namespaces#on_class_
          (Naming_elaborate_namespaces_endo.make_env env.namespace);
      elab_hints = Naming_elab_hints.elab_class;
      elab_collection = Naming_elab_collection.elab_class;
      elab_call = Naming_elab_call.elab_class;
      elab_tuple = Naming_elab_tuple.elab_class;
      elab_invariant = Naming_elab_invariant.elab_class;
      elab_const_expr = Naming_elab_const_expr.elab_class;
      elab_user_attrs = Naming_elab_user_attributes.elab_class;
      elab_help = class_help;
      elab_soft = Naming_elab_soft.elab_class;
      elab_everything_sdt = Naming_elab_everything_sdt.elab_class;
      elab_hkt = Naming_elab_hkt.elab_class;
      elab_enum_class = Naming_elab_enum_class.elab_class;
      elab_class_id = Naming_elab_class_id.elab_class;
      elab_dynamic_class_name = Naming_elab_dynamic_class_name.elab_class;
      validate_xhp = Naming_validate_xhp_name.validate_class;
      validate_builtin_enum = Naming_validate_builtin_enum.validate_class;
      validate_enum_class_typeconst =
        Naming_validate_enum_class_typeconst.validate_class;
      validate_supportdyn = Naming_validate_supportdyn.validate_class;
    }

let module_ ctx module_ =
  let env = Env.make_module_env ctx module_ in
  let filename = Pos.filename module_.Aast.md_span in
  elab_elem
    module_
    ~ctx
    ~env
    ~filename
    {
      elab_ns =
        elaborate_namespaces#on_module_def
          (Naming_elaborate_namespaces_endo.make_env env.namespace);
      elab_hints = Naming_elab_hints.elab_module_def;
      elab_collection = Naming_elab_collection.elab_module_def;
      elab_call = Naming_elab_call.elab_module_def;
      elab_tuple = Naming_elab_tuple.elab_module_def;
      elab_invariant = Naming_elab_invariant.elab_module_def;
      elab_const_expr = Naming_elab_const_expr.elab_module_def;
      elab_user_attrs = Naming_elab_user_attributes.elab_module_def;
      elab_help = module_help;
      elab_soft = Naming_elab_soft.elab_module_def;
      elab_everything_sdt = Naming_elab_everything_sdt.elab_module_def;
      elab_hkt = Naming_elab_hkt.elab_module_def;
      elab_enum_class = Naming_elab_enum_class.elab_module_def;
      elab_class_id = Naming_elab_class_id.elab_module_def;
      elab_dynamic_class_name = Naming_elab_dynamic_class_name.elab_module_def;
      validate_xhp = Naming_validate_xhp_name.validate_module_def;
      validate_builtin_enum = Naming_validate_builtin_enum.validate_module_def;
      validate_enum_class_typeconst =
        Naming_validate_enum_class_typeconst.validate_module_def;
      validate_supportdyn = Naming_validate_supportdyn.validate_module_def;
    }

let global_const ctx cst =
  let env = Env.make_const_env ctx cst in
  let filename = Pos.filename cst.Aast.cst_span in
  elab_elem
    cst
    ~ctx
    ~env
    ~filename
    {
      elab_ns =
        elaborate_namespaces#on_gconst
          (Naming_elaborate_namespaces_endo.make_env env.namespace);
      elab_hints = Naming_elab_hints.elab_gconst;
      elab_collection = Naming_elab_collection.elab_gconst;
      elab_call = Naming_elab_call.elab_gconst;
      elab_tuple = Naming_elab_tuple.elab_gconst;
      elab_invariant = Naming_elab_invariant.elab_gconst;
      elab_const_expr = Naming_elab_const_expr.elab_gconst;
      elab_user_attrs = Naming_elab_user_attributes.elab_gconst;
      elab_help = global_const_help;
      elab_soft = Naming_elab_soft.elab_gconst;
      elab_everything_sdt = Naming_elab_everything_sdt.elab_gconst;
      elab_hkt = Naming_elab_hkt.elab_gconst;
      elab_enum_class = Naming_elab_enum_class.elab_gconst;
      elab_class_id = Naming_elab_class_id.elab_gconst;
      elab_dynamic_class_name = Naming_elab_dynamic_class_name.elab_gconst;
      validate_xhp = Naming_validate_xhp_name.validate_gconst;
      validate_builtin_enum = Naming_validate_builtin_enum.validate_gconst;
      validate_enum_class_typeconst =
        Naming_validate_enum_class_typeconst.validate_gconst;
      validate_supportdyn = Naming_validate_supportdyn.validate_gconst;
    }

let typedef ctx tdef =
  let env = Env.make_typedef_env ctx tdef in
  let filename = Pos.filename @@ tdef.Aast.t_span in
  elab_elem
    tdef
    ~ctx
    ~env
    ~filename
    {
      elab_ns =
        elaborate_namespaces#on_typedef
          (Naming_elaborate_namespaces_endo.make_env env.namespace);
      elab_hints = Naming_elab_hints.elab_typedef;
      elab_collection = Naming_elab_collection.elab_typedef;
      elab_call = Naming_elab_call.elab_typedef;
      elab_tuple = Naming_elab_tuple.elab_typedef;
      elab_invariant = Naming_elab_invariant.elab_typedef;
      elab_const_expr = Naming_elab_const_expr.elab_typedef;
      elab_user_attrs = Naming_elab_user_attributes.elab_typedef;
      elab_help = typedef_help;
      elab_soft = Naming_elab_soft.elab_typedef;
      elab_everything_sdt = Naming_elab_everything_sdt.elab_typedef;
      elab_hkt = Naming_elab_hkt.elab_typedef;
      elab_enum_class = Naming_elab_enum_class.elab_typedef;
      elab_class_id = Naming_elab_class_id.elab_typedef;
      elab_dynamic_class_name = Naming_elab_dynamic_class_name.elab_typedef;
      validate_xhp = Naming_validate_xhp_name.validate_typedef;
      validate_builtin_enum = Naming_validate_builtin_enum.validate_typedef;
      validate_enum_class_typeconst =
        Naming_validate_enum_class_typeconst.validate_typedef;
      validate_supportdyn = Naming_validate_supportdyn.validate_typedef;
    }
