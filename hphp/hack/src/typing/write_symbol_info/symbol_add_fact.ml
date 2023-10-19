(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

open Aast
open Hh_prelude
open Symbol_glean_schema.Src
open Symbol_glean_schema.Hack
open Symbol_glean_schema.GenCode
module Util = Symbol_json_util
module Build_json = Symbol_build_json
module Predicate = Symbol_predicate
module Fact_id = Symbol_fact_id
module Fact_acc = Symbol_predicate.Fact_acc
module XRefs = Symbol_xrefs

let is_async = function
  | Ast_defs.FAsync -> true
  | Ast_defs.FAsyncGenerator -> true
  | _ -> false

let namespace_decl name fa =
  let json =
    NamespaceDeclaration.(
      { name = NamespaceQName.(Key (of_string name)) } |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack NamespaceDeclaration) json fa

(* create a namespace fact from a namespace attribute in node. Name can be empty
   in that case we simply ignore it *)
let namespace_decl_opt name fa =
  match name.Namespace_env.ns_name with
  | None -> fa (* global name space *)
  | Some "" -> fa
  | Some name -> namespace_decl name fa |> snd

let container_decl decl_pred name fa =
  let qname = QName.(Key (of_string name)) in
  let json =
    match decl_pred with
    | Predicate.(Hack ClassDeclaration) ->
      ClassDeclaration.(to_json_key { name = qname })
    | Predicate.(Hack EnumDeclaration) ->
      EnumDeclaration.(to_json_key { name = qname })
    | Predicate.(Hack InterfaceDeclaration) ->
      InterfaceDeclaration.(to_json_key { name = qname })
    | Predicate.(Hack TraitDeclaration) ->
      TraitDeclaration.(to_json_key { name = qname })
    | _ -> failwith "Impossible has happened"
  in
  Fact_acc.add_fact decl_pred json fa

let parent_decls ctx decls pred fa =
  List.fold decls ~init:([], fa) ~f:(fun (decl_refs, fa) decl ->
      let name = Util.strip_tparams (Util.get_type_from_hint ctx decl) in
      let (decl_id, fa) = container_decl pred name fa in
      (decl_id :: decl_refs, fa))

let parent_decls_enum ctx decls fa =
  let (fact_ids, fa) =
    parent_decls ctx decls Predicate.(Hack EnumDeclaration) fa
  in
  (List.map ~f:(fun x -> EnumDeclaration.Id x) fact_ids, fa)

let parent_decls_class ctx decls fa =
  let (fact_ids, fa) =
    parent_decls ctx decls Predicate.(Hack ClassDeclaration) fa
  in
  (List.map ~f:(fun x -> ClassDeclaration.Id x) fact_ids, fa)

let parent_decls_trait ctx decls fa =
  let (fact_ids, fa) =
    parent_decls ctx decls Predicate.(Hack TraitDeclaration) fa
  in
  (List.map ~f:(fun x -> TraitDeclaration.Id x) fact_ids, fa)

let parent_decls_interface ctx decls fa =
  let (fact_ids, fa) =
    parent_decls ctx decls Predicate.(Hack InterfaceDeclaration) fa
  in
  (List.map ~f:(fun x -> InterfaceDeclaration.Id x) fact_ids, fa)

let module_decl name fa =
  let json = ModuleDeclaration.({ name = Name.Key name } |> to_json_key) in
  Fact_acc.add_fact Predicate.(Hack ModuleDeclaration) json fa

let module_field module_ internal fa =
  match module_ with
  | None -> (None, fa)
  | Some (_pos, module_name) ->
    let (decl_id, fa) = module_decl module_name fa in
    (Some (Build_json.build_module_membership decl_id ~internal), fa)

let member_cluster ~members fa =
  let json = MemberCluster.({ members } |> to_json_key) in
  Fact_acc.add_fact Predicate.(Hack MemberCluster) json fa

let inherited_members ~container_type ~container_id ~member_clusters fa =
  let json =
    InheritedMembers.(
      {
        container = Predicate.container_decl container_type container_id;
        inheritedMembers =
          List.map ~f:(fun x -> MemberCluster.Id x) member_clusters;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack InheritedMembers) json fa

let container_defn ctx source_text clss decl_id members fa =
  let fa = namespace_decl_opt clss.c_namespace fa in
  let typeParams =
    List.map clss.c_tparams ~f:(Build_json.build_type_param ctx source_text)
  in
  let (module_, fa) = module_field clss.c_module clss.c_internal fa in
  let attributes =
    Build_json.build_attributes source_text clss.c_user_attributes
  in
  let (req_extends_hints, req_implements_hints, req_class_hints) =
    Aast.partition_map_require_kind ~f:(fun x -> x) clss.c_reqs
  in
  let (requireExtends, fa) =
    parent_decls_class ctx (List.map req_extends_hints ~f:fst) fa
  in
  let (requireImplements, fa) =
    parent_decls_interface ctx (List.map req_implements_hints ~f:fst) fa
  in
  let (requireClass, fa) =
    parent_decls_class ctx (List.map req_class_hints ~f:fst) fa
  in
  match Predicate.get_parent_kind clss.c_kind with
  | Predicate.InterfaceContainer ->
    let (extends_, fa) = parent_decls_interface ctx clss.c_extends fa in
    let json =
      InterfaceDefinition.(
        {
          declaration = InterfaceDeclaration.Id decl_id;
          typeParams;
          members;
          attributes;
          extends_;
          requireExtends;
          module_;
        }
        |> to_json_key)
    in
    Fact_acc.add_fact Predicate.(Hack InterfaceDefinition) json fa
  | Predicate.TraitContainer ->
    let (implements_, fa) = parent_decls_interface ctx clss.c_implements fa in
    let (uses, fa) = parent_decls_trait ctx clss.c_uses fa in
    let json =
      TraitDefinition.(
        {
          declaration = TraitDeclaration.Id decl_id;
          members;
          implements_;
          uses;
          attributes;
          typeParams;
          requireExtends;
          requireImplements;
          requireClass = Some requireClass;
          module_;
        }
        |> to_json_key)
    in
    Fact_acc.add_fact Predicate.(Hack TraitDefinition) json fa
  | Predicate.ClassContainer ->
    let (implements_, fa) = parent_decls_interface ctx clss.c_implements fa in
    let (uses, fa) = parent_decls_trait ctx clss.c_uses fa in
    let (extends_, fa) =
      match clss.c_extends with
      | [] -> (None, fa)
      | [parent] ->
        let (decl_id, fa) =
          let parent_clss =
            Util.strip_tparams (Util.get_type_from_hint ctx parent)
          in
          let qname = QName.(Key (of_string parent_clss)) in
          let json = ClassDeclaration.(to_json_key { name = qname }) in
          Fact_acc.add_fact Predicate.(Hack ClassDeclaration) json fa
        in
        (Some (ClassDeclaration.Id decl_id), fa)
      | _ ->
        Hh_logger.log
          "WARNING: skipping extends field for class with multiple parents %s"
          (snd clss.c_name);
        (None, fa)
    in
    let json =
      ClassDefinition.(
        {
          declaration = ClassDeclaration.Id decl_id;
          isAbstract = Ast_defs.is_classish_abstract clss.c_kind;
          isFinal = clss.c_final;
          members;
          extends_;
          implements_;
          uses;
          attributes;
          typeParams;
          module_;
        }
        |> to_json_key)
    in
    Fact_acc.add_fact Predicate.(Hack ClassDefinition) json fa

let property_decl con_type decl_id name fa =
  let json =
    PropertyDeclaration.(
      {
        name = Name.Key name;
        container = Predicate.container_decl con_type decl_id;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack PropertyDeclaration) json fa

let class_const_decl con_type decl_id name fa =
  let json =
    ClassConstDeclaration.(
      {
        name = Name.Key name;
        container = Predicate.container_decl con_type decl_id;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack ClassConstDeclaration) json fa

let type_const_decl con_type decl_id name fa =
  let json =
    TypeConstDeclaration.(
      {
        name = Name.Key name;
        container = Predicate.container_decl con_type decl_id;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack TypeConstDeclaration) json fa

let method_decl con_type decl_id name fa =
  let json =
    MethodDeclaration.(
      {
        name = Name.Key name;
        container = Predicate.container_decl con_type decl_id;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack MethodDeclaration) json fa

let type_info ~ty sym_pos fa =
  let json =
    TypeInfo.(
      Key.
        {
          displayType = Type.Key ty;
          xrefs = Build_json.build_hint_xrefs sym_pos;
        }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack TypeInfo) json fa

let aggregate_pos (json_pos_list : (XRefTarget.t * Util.pos) list) :
    (XRefTarget.t * Util.pos list) list =
  let jmap =
    List.fold json_pos_list ~init:Map.Poly.empty ~f:(fun acc (json, pos) ->
        let f = function
          | None -> [pos]
          | Some prev -> pos :: prev
        in
        Map.Poly.update acc json ~f)
  in
  Map.Poly.to_alist jmap

let build_signature ctx pos_map_opt source_text params ctxs ret fa =
  let pos_map =
    match pos_map_opt with
    | Some pos_map -> pos_map
    | None -> failwith "Internal error: pos_map should be set in previous phase"
  in
  let hint_to_str_opt h fa =
    match hint_of_type_hint h with
    | None -> (None, None, fa)
    | Some hint ->
      let legacy_ty = Util.get_type_from_hint ctx hint in
      let (ty, sym_pos) = Util.hint_to_string_and_symbols ctx hint in
      let decl_json_pos =
        List.filter_map sym_pos ~f:(fun (source_pos, pos) ->
            match XRefs.PosMap.find_opt source_pos pos_map with
            | Some (XRefs.{ target; _ } :: _) -> Some (target, pos)
            | _ -> None)
      in
      let decl_json_aggr_pos = aggregate_pos decl_json_pos in
      let (fact_id, fa) = type_info ~ty decl_json_aggr_pos fa in
      (Some legacy_ty, Some fact_id, fa)
  in
  let (params, fa) =
    List.fold params ~init:([], fa) ~f:(fun (t_params, fa) p ->
        let (p_ty, fact_id, fa) = hint_to_str_opt p.param_type_hint fa in
        ((p, fact_id, p_ty) :: t_params, fa))
  in
  let params = List.rev params in
  let (ret_ty, return_info, fa) = hint_to_str_opt ret fa in
  let signature =
    Build_json.build_signature ctx source_text params ctxs ~ret_ty ~return_info
  in
  (signature, fa)

let method_defn ctx source_text meth decl_id fa =
  let m_tparams = Util.remove_generated_tparams meth.m_tparams in
  let typeParams =
    List.map m_tparams ~f:(Build_json.build_type_param ctx source_text)
  in
  let (signature, fa) =
    build_signature
      ctx
      (Fact_acc.get_pos_map fa)
      source_text
      meth.m_params
      meth.m_ctxs
      meth.m_ret
      fa
  in
  let readonlyRet =
    Option.map meth.m_readonly_ret ~f:(fun _ -> ReadonlyKind.ReadOnly)
  in
  let isReadonlyThis =
    if meth.m_readonly_this then
      Some meth.m_readonly_this
    else
      None
  in
  let json =
    MethodDefinition.(
      {
        declaration = MethodDeclaration.Id decl_id;
        signature;
        visibility = Visibility.(of_visibility meth.m_visibility);
        isAbstract = meth.m_abstract;
        isAsync = is_async meth.m_fun_kind;
        isFinal = meth.m_final;
        isStatic = meth.m_static;
        attributes =
          Build_json.build_attributes source_text meth.m_user_attributes;
        typeParams;
        readonlyRet;
        isReadonlyThis;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack MethodDefinition) json fa

let method_overrides
    meth_name base_cont_name base_cont_type der_cont_name der_cont_type fa =
  let json =
    MethodOverrides.(
      Key.
        {
          derived =
            Build_json.build_method_decl meth_name der_cont_name der_cont_type;
          base =
            Build_json.build_method_decl meth_name base_cont_name base_cont_type;
        }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack MethodOverrides) json fa

let property_defn ctx source_text prop decl_id fa =
  let type_ =
    Option.map
      ~f:(fun x -> Type.Key (Util.get_type_from_hint ctx x))
      (hint_of_type_hint prop.cv_type)
  in
  let json =
    PropertyDefinition.(
      {
        declaration = PropertyDeclaration.Id decl_id;
        type_ : Type.t option;
        visibility = Visibility.(of_visibility prop.cv_visibility);
        isFinal = prop.cv_final;
        isAbstract = prop.cv_abstract;
        isStatic = prop.cv_is_static;
        attributes =
          Build_json.build_attributes source_text prop.cv_user_attributes;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack PropertyDefinition) json fa

let class_const_defn ctx source_text const decl_id fa =
  let value =
    match const.cc_kind with
    | CCAbstract None -> None
    | CCAbstract (Some expr)
    | CCConcrete expr ->
      Some (Util.ast_expr_to_string_stripped source_text expr)
  in
  let type_ =
    Option.map
      ~f:(fun x -> Type.Key (Util.get_type_from_hint ctx x))
      const.cc_type
  in
  let json =
    ClassConstDefinition.(
      { declaration = ClassConstDeclaration.Id decl_id; type_; value }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack ClassConstDefinition) json fa

let type_const_defn ctx source_text tc decl_id fa =
  (* TODO(T88552052) should the default of an abstract type constant be used
     * as a value here *)
  let type_ =
    match tc.c_tconst_kind with
    | TCConcrete { c_tc_type = h }
    | TCAbstract { c_atc_default = Some h; _ } ->
      Some (Type.Key (Util.get_type_from_hint ctx h))
    | TCAbstract { c_atc_default = None; _ } -> None
  in
  let json =
    TypeConstDefinition.(
      {
        declaration = TypeConstDeclaration.Id decl_id;
        kind = TypeConstKind.(of_ast_const_kind tc.c_tconst_kind);
        type_;
        attributes =
          Build_json.build_attributes source_text tc.c_tconst_user_attributes;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack TypeConstDefinition) json fa

let enum_decl name fa =
  let json =
    EnumDeclaration.({ name = QName.(Key (of_string name)) } |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack EnumDeclaration) json fa

let enum_defn ctx source_text enm enum_id enum_data enumerators fa =
  let enumerators = List.map enumerators ~f:(fun x -> Enumerator.Id x) in
  let fa = namespace_decl_opt enm.c_namespace fa in
  let (includes, fa) = parent_decls_enum ctx enum_data.e_includes fa in
  let (module_, fa) = module_field enm.c_module enm.c_internal fa in
  let enumConstraint =
    Option.map enum_data.e_constraint ~f:(fun x ->
        Type.Key (Util.get_type_from_hint ctx x))
  in
  let json =
    EnumDefinition.(
      {
        declaration = EnumDeclaration.Id enum_id;
        enumBase = Type.Key (Util.get_type_from_hint ctx enum_data.e_base);
        enumerators;
        attributes =
          Build_json.build_attributes source_text enm.c_user_attributes;
        includes;
        isEnumClass = Aast.is_enum_class enm;
        module_;
        enumConstraint;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack EnumDefinition) json fa

let enumerator decl_id const_name fa =
  let json =
    Enumerator.(
      { name = Name.Key const_name; enumeration = EnumDeclaration.Id decl_id }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack Enumerator) json fa

let func_decl name fa =
  let json =
    FunctionDeclaration.({ name = QName.(Key (of_string name)) } |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack FunctionDeclaration) json fa

let func_defn ctx source_text fd decl_id fa =
  let elem = fd.fd_fun in
  let fa = namespace_decl_opt fd.fd_namespace fa in
  let fd_tparams = Util.remove_generated_tparams fd.fd_tparams in
  let typeParams =
    List.map fd_tparams ~f:(Build_json.build_type_param ctx source_text)
  in
  let (module_, fa) = module_field fd.fd_module fd.fd_internal fa in
  let (signature, fa) =
    build_signature
      ctx
      (Fact_acc.get_pos_map fa)
      source_text
      elem.f_params
      elem.f_ctxs
      elem.f_ret
      fa
  in
  let readonlyRet =
    Option.map elem.f_readonly_ret ~f:(fun _ -> ReadonlyKind.ReadOnly)
  in
  let json =
    FunctionDefinition.(
      {
        declaration = FunctionDeclaration.Id decl_id;
        signature;
        isAsync = is_async elem.f_fun_kind;
        attributes =
          Build_json.build_attributes source_text elem.f_user_attributes;
        typeParams;
        readonlyRet;
        module_;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack FunctionDefinition) json fa

let module_defn _ctx source_text elem decl_id fa =
  let json =
    ModuleDefinition.(
      {
        declaration = ModuleDeclaration.Id decl_id;
        attributes =
          Build_json.build_attributes source_text elem.md_user_attributes;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack ModuleDefinition) json fa

let typedef_decl name fa =
  let json =
    TypedefDeclaration.({ name = QName.(Key (of_string name)) } |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack TypedefDeclaration) json fa

let typedef_defn ctx source_text elem decl_id fa =
  let fa = namespace_decl_opt elem.t_namespace fa in
  let isTransparent =
    match elem.t_vis with
    | Transparent -> true
    | CaseType
    | Opaque
    | OpaqueModule ->
      false
  in
  let typeParams =
    List.map elem.t_tparams ~f:(Build_json.build_type_param ctx source_text)
  in
  let (module_, fa) = module_field elem.t_module elem.t_internal fa in
  let json =
    TypedefDefinition.(
      {
        declaration = TypedefDeclaration.Id decl_id;
        isTransparent;
        attributes =
          Build_json.build_attributes source_text elem.t_user_attributes;
        typeParams;
        module_;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack TypedefDefinition) json fa

let gconst_decl name fa =
  let json =
    GlobalConstDeclaration.(
      { name = QName.(Key (of_string name)) } |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack GlobalConstDeclaration) json fa

let gconst_defn ctx source_text elem decl_id fa =
  let fa = namespace_decl_opt elem.cst_namespace fa in
  let value = Util.ast_expr_to_string_stripped source_text elem.cst_value in
  let type_ =
    Option.map elem.cst_type ~f:(fun x ->
        Type.Key (Util.get_type_from_hint ctx x))
  in
  let json =
    GlobalConstDefinition.(
      { declaration = GlobalConstDeclaration.Id decl_id; type_; value }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack GlobalConstDefinition) json fa

let decl_loc ~path pos declaration fa =
  let json =
    DeclarationLocation.(
      { declaration; file = File.(Key path); span = ByteSpan.(of_pos pos) }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack DeclarationLocation) json fa

let decl_comment ~path pos declaration fa =
  let json =
    DeclarationComment.(
      { declaration; file = File.Key path; span = ByteSpan.of_pos pos }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack DeclarationComment) json fa

let decl_span ~path pos declaration fa =
  let json =
    DeclarationSpan.(
      { declaration; file = File.Key path; span = ByteSpan.of_pos pos }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack DeclarationSpan) json fa

let file_lines ~path sourceText fa =
  let lengths =
    Line_break_map.offsets_to_line_lengths
      sourceText.Full_fidelity_source_text.offset_map
  in
  let endsInNewline = Util.ends_in_newline sourceText in
  let hasUnicodeOrTabs = Util.has_tabs_or_multibyte_codepoints sourceText in
  let json =
    FileLines.(
      to_json_key
        { file = File.Key path; lengths; endsInNewline; hasUnicodeOrTabs })
  in
  Fact_acc.add_fact Predicate.(Src FileLines) json fa

let file_xrefs ~path fact_map fa =
  let json =
    FileXRefs.(
      Key.{ file = File.Key path; xrefs = Build_json.build_xrefs fact_map }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack FileXRefs) json fa

let file_decls ~path declarations fa =
  let json =
    FileDeclarations.(Key.{ file = File.Key path; declarations } |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack FileDeclarations) json fa

let method_occ receiver_class name fa =
  let json =
    MethodOccurrence.(
      {
        name = Name.Key name;
        className =
          (match receiver_class with
          | SymbolOccurrence.UnknownClass -> None
          | SymbolOccurrence.ClassName className -> Some (Name.Key className));
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack MethodOccurrence) json fa

let file_call ~path pos ~callee_infos ~call_args ~dispatch_arg fa =
  let callee_xrefs = List.map callee_infos ~f:(fun ti -> ti.XRefs.target) in
  let callee_xref =
    match List.sort ~compare:XRefTarget.compare callee_xrefs with
    | [] -> None
    | hd :: _ -> Some hd
  in
  let receiver_type =
    List.find_map callee_infos ~f:(fun ti -> ti.XRefs.receiver_type)
  in
  let json =
    FileCall.(
      {
        file = File.Key path;
        callee_span = ByteSpan.(of_pos pos);
        callee_xref;
        dispatch_arg;
        receiver_type;
        call_args;
        callee_xrefs;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack FileCall) json fa

let global_namespace_alias ~from ~to_ fa =
  let json =
    GlobalNamespaceAlias.(
      { from = Name.(Key from); to_ = NamespaceQName.(Key (of_string to_)) }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Hack GlobalNamespaceAlias) json fa

let indexerInputsHash key hashes fa =
  let key = IndexerInputHash.(key |> to_json_key) in
  let value =
    IndexerInputHash.(List.map hashes ~f:Md5.to_binary |> to_json_value)
  in
  Fact_acc.add_fact Predicate.(Hack IndexerInputsHash) key ~value fa

let gen_code ~path ~fully_generated ~signature ~source ~command ~class_ fa =
  let json =
    GenCode.(
      {
        file = File.Key path;
        variant =
          (if fully_generated then
            GenCodeVariant.Full
          else
            GenCodeVariant.Partial);
        source = Option.map ~f:(fun x -> File.Key x) source;
        signature = Option.map ~f:(fun x -> GenCodeSignature.Key x) signature;
        command = Option.map ~f:(fun x -> GenCodeCommand.Key x) command;
        class_ = Option.map ~f:(fun x -> GenCodeClass.Key x) class_;
      }
      |> to_json_key)
  in
  Fact_acc.add_fact Predicate.(Gencode GenCode) json fa
