// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the "hack" directory of this source tree.

use super::{folded, shallow, ty::*};
use crate::reason::Reason;
use oxidized_by_ref::ast::Id;
use pos::{Pos, ToOxidized};

use oxidized_by_ref as obr;

impl<'a> ToOxidized<'a> for CeVisibility {
    type Output = obr::typing_defs::CeVisibility<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        use obr::typing_defs::CeVisibility as Obr;
        match self {
            CeVisibility::Public => Obr::Vpublic,
            CeVisibility::Private(v) => Obr::Vprivate(v.to_oxidized(arena)),
            CeVisibility::Protected(v) => Obr::Vprotected(v.to_oxidized(arena)),
            CeVisibility::Internal(v) => Obr::Vinternal(v.to_oxidized(arena)),
        }
    }
}

impl<'a> ToOxidized<'a> for IfcFunDecl {
    type Output = obr::typing_defs::IfcFunDecl<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        use obr::typing_defs::IfcFunDecl as Obr;
        match self {
            IfcFunDecl::FDPolicied(x) => Obr::FDPolicied(x.to_oxidized(arena)),
            IfcFunDecl::FDInferFlows => Obr::FDInferFlows,
        }
    }
}

impl<'a, P: Pos> ToOxidized<'a> for UserAttribute<P> {
    type Output = &'a obr::typing_defs::UserAttribute<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::UserAttribute {
            name: self.name.to_oxidized(arena),
            classname_params: self.classname_params.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for Tparam<R, Ty<R>> {
    type Output = &'a obr::typing_defs::Tparam<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::Tparam {
            variance: self.variance,
            name: self.name.to_oxidized(arena),
            tparams: self.tparams.to_oxidized(arena),
            constraints: arena.alloc_slice_fill_iter(
                self.constraints
                    .iter()
                    .map(|(x, y)| (*x, y.to_oxidized(arena))),
            ),
            reified: self.reified,
            user_attributes: self.user_attributes.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for WhereConstraint<Ty<R>> {
    type Output = &'a obr::typing_defs::WhereConstraint<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        let WhereConstraint(tvar_ty, kind, as_ty) = self;
        arena.alloc(obr::typing_defs::WhereConstraint(
            tvar_ty.to_oxidized(arena),
            *kind,
            as_ty.to_oxidized(arena),
        ))
    }
}

impl<'a, R: Reason> ToOxidized<'a> for Ty<R> {
    type Output = &'a obr::typing_defs::Ty<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::Ty(
            arena.alloc(self.reason().to_oxidized(arena)),
            self.node().to_oxidized(arena),
        ))
    }
}

impl<'a, R: Reason> ToOxidized<'a> for ShapeFieldType<R> {
    type Output = &'a obr::typing_defs::ShapeFieldType<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::ShapeFieldType {
            optional: self.optional,
            ty: self.ty.to_oxidized(arena),
        })
    }
}

fn oxidize_shape_field_name<'a, P: Pos>(
    arena: &'a bumpalo::Bump,
    name: TshapeFieldName,
    field_name_pos: &ShapeFieldNamePos<P>,
) -> obr::typing_defs::TshapeFieldName<'a> {
    use obr::typing_defs::TshapeFieldName as Obr;
    use ShapeFieldNamePos as SfnPos;
    let simple_pos = || match field_name_pos {
        SfnPos::Simple(p) => p.to_oxidized(arena),
        SfnPos::ClassConst(..) => panic!("expected ShapeFieldNamePos::Simple"),
    };
    match name {
        TshapeFieldName::TSFlitInt(x) => Obr::TSFlitInt(arena.alloc(obr::typing_defs::PosString(
            simple_pos(),
            x.to_oxidized(arena),
        ))),
        TshapeFieldName::TSFlitStr(x) => Obr::TSFlitStr(arena.alloc(
            obr::typing_defs::PosByteString(simple_pos(), x.to_oxidized(arena).into()),
        )),
        TshapeFieldName::TSFclassConst(cls, name) => {
            let (pos1, pos2) = match field_name_pos {
                SfnPos::ClassConst(p1, p2) => (p1.to_oxidized(arena), p2.to_oxidized(arena)),
                SfnPos::Simple(..) => panic!("expected ShapeFieldNamePos::ClassConst"),
            };
            Obr::TSFclassConst(arena.alloc((
                (pos1, cls.to_oxidized(arena)),
                obr::typing_defs::PosString(pos2, name.to_oxidized(arena)),
            )))
        }
    }
}

impl<'a, R: Reason> ToOxidized<'a> for Ty_<R> {
    type Output = obr::typing_defs::Ty_<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        use obr::t_shape_map::{TShapeField, TShapeMap};
        use obr::typing_defs;
        match self {
            Ty_::Tthis => typing_defs::Ty_::Tthis,
            Ty_::Tapply(x) => typing_defs::Ty_::Tapply(x.to_oxidized(arena)),
            Ty_::Tmixed => typing_defs::Ty_::Tmixed,
            Ty_::Tlike(x) => typing_defs::Ty_::Tlike(x.to_oxidized(arena)),
            Ty_::Tany => typing_defs::Ty_::Tany(obr::tany_sentinel::TanySentinel),
            Ty_::Terr => typing_defs::Ty_::Terr,
            Ty_::Tnonnull => typing_defs::Ty_::Tnonnull,
            Ty_::Tdynamic => typing_defs::Ty_::Tdynamic,
            Ty_::Toption(x) => typing_defs::Ty_::Toption(x.to_oxidized(arena)),
            Ty_::Tprim(x) => typing_defs::Ty_::Tprim(arena.alloc(*x)),
            Ty_::Tfun(x) => typing_defs::Ty_::Tfun(x.to_oxidized(arena)),
            Ty_::Ttuple(x) => typing_defs::Ty_::Ttuple(x.to_oxidized(arena)),
            Ty_::Tshape(shape) => {
                let mut shape_fields = arena_collections::AssocListMut::new_in(arena);
                let (shape_kind, shape_field_type_map): &(_, _) = shape;
                for (k, v) in shape_field_type_map.iter() {
                    let k = oxidize_shape_field_name(arena, *k, &v.field_name_pos);
                    shape_fields.insert_or_replace(TShapeField(k), v.to_oxidized(arena));
                }
                typing_defs::Ty_::Tshape(arena.alloc((*shape_kind, TShapeMap::from(shape_fields))))
            }
            Ty_::Tvar(ident) => typing_defs::Ty_::Tvar((*ident).into()),
            Ty_::Tgeneric(x) => typing_defs::Ty_::Tgeneric(x.to_oxidized(arena)),
            Ty_::Tunion(x) => typing_defs::Ty_::Tunion(x.to_oxidized(arena)),
            Ty_::Tintersection(x) => typing_defs::Ty_::Tintersection(x.to_oxidized(arena)),
            Ty_::TvecOrDict(x) => typing_defs::Ty_::TvecOrDict(x.to_oxidized(arena)),
            Ty_::Taccess(x) => typing_defs::Ty_::Taccess(x.to_oxidized(arena)),
        }
    }
}

impl<'a, R: Reason> ToOxidized<'a> for TaccessType<R, Ty<R>> {
    type Output = &'a obr::typing_defs::TaccessType<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::TaccessType(
            self.ty.to_oxidized(arena),
            self.type_const.to_oxidized(arena),
        ))
    }
}

impl<'a, R: Reason> ToOxidized<'a> for FunImplicitParams<R, Ty<R>> {
    type Output = &'a obr::typing_defs::FunImplicitParams<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::FunImplicitParams {
            capability: match &self.capability {
                Capability::CapDefaults(p) => {
                    obr::typing_defs::Capability::CapDefaults(p.to_oxidized(arena))
                }
                Capability::CapTy(ty) => obr::typing_defs::Capability::CapTy(ty.to_oxidized(arena)),
            },
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for FunType<R, Ty<R>> {
    type Output = &'a obr::typing_defs::FunType<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::FunType {
            tparams: self.tparams.to_oxidized(arena),
            where_constraints: self.where_constraints.to_oxidized(arena),
            params: self.params.to_oxidized(arena),
            implicit_params: self.implicit_params.to_oxidized(arena),
            ret: self.ret.to_oxidized(arena),
            flags: self.flags,
            ifc_decl: self.ifc_decl.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for PossiblyEnforcedTy<Ty<R>> {
    type Output = &'a obr::typing_defs::PossiblyEnforcedTy<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::PossiblyEnforcedTy {
            enforced: self.enforced,
            type_: self.ty.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for FunParam<R, Ty<R>> {
    type Output = &'a obr::typing_defs::FunParam<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::FunParam {
            pos: self.pos.to_oxidized(arena),
            name: self.name.to_oxidized(arena),
            type_: self.ty.to_oxidized(arena),
            flags: self.flags,
        })
    }
}

impl<'a> ToOxidized<'a> for XhpEnumValue {
    type Output = obr::ast_defs::XhpEnumValue<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        use obr::ast_defs::XhpEnumValue as Obr;
        match self {
            Self::XEVInt(i) => Obr::XEVInt(*i),
            Self::XEVString(s) => Obr::XEVString(s.to_oxidized(arena)),
        }
    }
}

impl<'a> ToOxidized<'a> for ClassConstFrom {
    type Output = obr::typing_defs::ClassConstFrom<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        use obr::typing_defs::ClassConstFrom as Obr;
        match self {
            Self::Self_ => Obr::Self_,
            Self::From(ty) => Obr::From(ty.to_oxidized(arena)),
        }
    }
}

impl<'a> ToOxidized<'a> for ClassConstRef {
    type Output = obr::typing_defs::ClassConstRef<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        let ClassConstRef(class, symbol) = self;
        obr::typing_defs::ClassConstRef(class.to_oxidized(arena), symbol.to_oxidized(arena))
    }
}

impl<'a, R: Reason> ToOxidized<'a> for AbstractTypeconst<R> {
    type Output = &'a obr::typing_defs::AbstractTypeconst<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::AbstractTypeconst {
            as_constraint: self.as_constraint.to_oxidized(arena),
            super_constraint: self.super_constraint.to_oxidized(arena),
            default: self.default.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for ConcreteTypeconst<R> {
    type Output = &'a obr::typing_defs::ConcreteTypeconst<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::ConcreteTypeconst {
            tc_type: self.ty.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for Typeconst<R> {
    type Output = obr::typing_defs::Typeconst<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        use obr::typing_defs::Typeconst as Obr;
        match self {
            Self::TCAbstract(x) => Obr::TCAbstract(x.to_oxidized(arena)),
            Self::TCConcrete(x) => Obr::TCConcrete(x.to_oxidized(arena)),
        }
    }
}

impl<'a, R: Reason> ToOxidized<'a> for EnumType<R> {
    type Output = &'a obr::typing_defs::EnumType<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::EnumType {
            base: self.base.to_oxidized(arena),
            constraint: self.constraint.as_ref().map(|c| c.to_oxidized(arena)),
            includes: self.includes.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for folded::SubstContext<R> {
    type Output = &'a obr::decl_defs::SubstContext<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        let subst = &self.subst.0;
        arena.alloc(obr::decl_defs::SubstContext {
            subst: subst.to_oxidized(arena),
            class_context: self.class_context.to_oxidized(arena),
            from_req_extends: self.from_req_extends,
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for folded::TypeConst<R> {
    type Output = &'a obr::typing_defs::TypeconstType<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::TypeconstType {
            synthesized: self.is_synthesized,
            concretized: self.is_concretized,
            is_ctx: self.is_ctx,
            enforceable: self.enforceable.as_ref().map_or_else(
                || (obr::pos::Pos::none(), false),
                |x| (x.to_oxidized(arena), true),
            ),
            reifiable: self.reifiable.as_ref().map(|x| x.to_oxidized(arena)),
            origin: self.origin.to_oxidized(arena),
            kind: self.kind.to_oxidized(arena),
            name: self.name.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for folded::ClassConst<R> {
    type Output = &'a obr::typing_defs::ClassConst<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::typing_defs::ClassConst {
            synthesized: self.is_synthesized,
            abstract_: self.kind,
            origin: self.origin.to_oxidized(arena),
            refs: self.refs.to_oxidized(arena),
            type_: self.ty.to_oxidized(arena),
            pos: self.pos.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for folded::Requirement<R> {
    type Output = &'a obr::decl_defs::Requirement<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::decl_defs::Requirement(
            self.pos.to_oxidized(arena),
            self.ty.to_oxidized(arena),
        ))
    }
}

impl<'a> ToOxidized<'a> for folded::Constructor {
    type Output = (Option<&'a obr::decl_defs::Element<'a>>, ConsistentKind);

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        (self.elt.to_oxidized(arena), self.consistency)
    }
}

impl<'a, R: Reason> ToOxidized<'a> for folded::FoldedClass<R> {
    type Output = obr::decl_defs::DeclClassType<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        // Destructure to help ensure we convert every field.
        let Self {
            name,
            pos,
            kind,
            is_final,
            is_const,
            is_internal,
            is_xhp,
            has_xhp_keyword,
            support_dynamic_type,
            module,
            tparams,
            where_constraints,
            substs,
            ancestors,
            props,
            static_props,
            methods,
            static_methods,
            consts,
            type_consts,
            xhp_enum_values,
            constructor,
            deferred_init_members,
            req_ancestors,
            req_ancestors_extends,
            req_class_ancestors,
            extends,
            sealed_whitelist,
            xhp_attr_deps,
            enum_type,
            decl_errors,
        } = self;
        obr::decl_defs::DeclClassType {
            name: name.to_oxidized(arena),
            pos: pos.to_oxidized(arena),
            kind: *kind,
            abstract_: self.is_abstract(),
            final_: *is_final,
            const_: *is_const,
            internal: *is_internal,
            is_xhp: *is_xhp,
            has_xhp_keyword: *has_xhp_keyword,
            support_dynamic_type: *support_dynamic_type,
            module: module.as_ref().map(|m| {
                let (pos, id) = m.to_oxidized(arena);
                Id(pos, id)
            }),
            tparams: tparams.to_oxidized(arena),
            where_constraints: where_constraints.to_oxidized(arena),
            substs: substs.to_oxidized(arena),
            ancestors: ancestors.to_oxidized(arena),
            props: props.to_oxidized(arena),
            sprops: static_props.to_oxidized(arena),
            methods: methods.to_oxidized(arena),
            smethods: static_methods.to_oxidized(arena),
            consts: consts.to_oxidized(arena),
            typeconsts: type_consts.to_oxidized(arena),
            xhp_enum_values: xhp_enum_values.to_oxidized(arena),
            construct: constructor.to_oxidized(arena),
            need_init: self.has_concrete_constructor(),
            deferred_init_members: deferred_init_members.to_oxidized(arena),
            req_ancestors: req_ancestors.to_oxidized(arena),
            req_ancestors_extends: req_ancestors_extends.to_oxidized(arena),
            req_class_ancestors: req_class_ancestors.to_oxidized(arena),
            extends: extends.to_oxidized(arena),
            sealed_whitelist: sealed_whitelist.to_oxidized(arena),
            xhp_attr_deps: xhp_attr_deps.to_oxidized(arena),
            enum_type: enum_type.as_ref().map(|et| et.to_oxidized(arena)),
            decl_errors: decl_errors.to_oxidized(arena),
        }
    }
}

impl<'a> ToOxidized<'a> for folded::FoldedElement {
    type Output = &'a obr::decl_defs::Element<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        arena.alloc(obr::decl_defs::Element {
            origin: self.origin.to_oxidized(arena),
            visibility: self.visibility.to_oxidized(arena),
            deprecated: self.deprecated.map(|x| {
                bumpalo::collections::String::from_utf8_lossy_in(x.as_bytes(), arena)
                    .into_bump_str()
            }),
            flags: self.flags,
        })
    }
}

impl<'a, P: ToOxidized<'a, Output = &'a obr::pos::Pos<'a>>> ToOxidized<'a>
    for crate::decl_error::DeclError<P>
{
    type Output = obr::decl_defs::DeclError<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        use obr::decl_defs::DeclError;
        match self {
            &Self::WrongExtendKind {
                ref pos,
                kind,
                name,
                ref parent_pos,
                parent_kind,
                parent_name,
            } => DeclError::WrongExtendKind {
                pos: pos.to_oxidized(arena),
                kind,
                name: name.to_oxidized(arena),
                parent_pos: parent_pos.to_oxidized(arena),
                parent_kind,
                parent_name: parent_name.to_oxidized(arena),
            },
            Self::CyclicClassDef(pos, stack) => DeclError::CyclicClassDef {
                pos: pos.to_oxidized(arena),
                stack: oxidized_by_ref::s_set::SSet::from(
                    arena,
                    stack.iter().map(|s| s.to_oxidized(arena)),
                ),
            },
        }
    }
}

impl<'a, R: Reason> ToOxidized<'a> for shallow::ClassDecl<R> {
    type Output = &'a obr::shallow_decl_defs::ClassDecl<'a>;

    fn to_oxidized(&self, _arena: &'a bumpalo::Bump) -> Self::Output {
        todo!()
    }
}

impl<'a, R: Reason> ToOxidized<'a> for shallow::FunDecl<R> {
    type Output = &'a obr::shallow_decl_defs::FunDecl<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        let Self {
            deprecated,
            module,
            internal,
            ty,
            pos,
            php_std_lib,
            support_dynamic_type,
        } = self;
        arena.alloc(obr::shallow_decl_defs::FunDecl {
            deprecated: deprecated.as_ref().map(|s| {
                bumpalo::collections::String::from_utf8_lossy_in(s.as_bytes(), arena)
                    .into_bump_str()
            }),
            internal: *internal,
            type_: ty.to_oxidized(arena),
            pos: pos.to_oxidized(arena),
            php_std_lib: *php_std_lib,
            support_dynamic_type: *support_dynamic_type,
            module: module.as_ref().map(|m| {
                let (pos, id) = m.to_oxidized(arena);
                oxidized_by_ref::ast_defs::Id(pos, id)
            }),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for shallow::TypedefDecl<R> {
    type Output = &'a obr::shallow_decl_defs::TypedefDecl<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        let Self {
            module,
            pos,
            vis,
            tparams,
            constraint,
            ty,
            is_ctx,
            attributes,
            internal,
        } = self;
        arena.alloc(obr::shallow_decl_defs::TypedefDecl {
            module: module.as_ref().map(|m| {
                let (pos, id) = m.to_oxidized(arena);
                oxidized_by_ref::ast_defs::Id(pos, id)
            }),
            pos: pos.to_oxidized(arena),
            vis: *vis,
            tparams: tparams.to_oxidized(arena),
            constraint: constraint.as_ref().map(|t| t.to_oxidized(arena)),
            type_: ty.to_oxidized(arena),
            is_ctx: *is_ctx,
            attributes: attributes.to_oxidized(arena),
            internal: *internal,
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for shallow::ConstDecl<R> {
    type Output = &'a obr::shallow_decl_defs::ConstDecl<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        let Self { pos, ty } = self;
        arena.alloc(obr::shallow_decl_defs::ConstDecl {
            pos: pos.to_oxidized(arena),
            type_: ty.to_oxidized(arena),
        })
    }
}

impl<'a, R: Reason> ToOxidized<'a> for shallow::ModuleDecl<R> {
    type Output = &'a obr::shallow_decl_defs::ModuleDecl<'a>;

    fn to_oxidized(&self, arena: &'a bumpalo::Bump) -> Self::Output {
        let Self { pos } = self;
        arena.alloc(obr::shallow_decl_defs::ModuleDecl {
            mdt_pos: pos.to_oxidized(arena),
        })
    }
}
