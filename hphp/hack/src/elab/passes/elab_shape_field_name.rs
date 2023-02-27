// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the "hack" directory of this source tree.
use std::ops::ControlFlow;

use naming_special_names_rust as sn;
use oxidized::aast_defs::Class_;
use oxidized::aast_defs::Expr_;
use oxidized::aast_defs::ShapeFieldInfo;
use oxidized::naming_error::NamingError;
use oxidized::naming_phase_error::NamingPhaseError;
use oxidized::nast::ShapeFieldName;

use crate::config::Config;
use crate::Pass;

#[derive(Clone, Default)]
pub struct ElabShapeFieldNamePass {
    current_class: Option<String>,
}

impl ElabShapeFieldNamePass {
    pub fn in_class<Ex, En>(&mut self, cls: &Class_<Ex, En>) {
        self.current_class = Some(cls.name.name().to_string())
    }
}

impl Pass for ElabShapeFieldNamePass {
    fn on_ty_class__top_down<Ex, En>(
        &mut self,
        elem: &mut Class_<Ex, En>,
        _cfg: &Config,
        _errs: &mut Vec<NamingPhaseError>,
    ) -> ControlFlow<(), ()>
    where
        Ex: Default,
    {
        self.in_class(elem);
        ControlFlow::Continue(())
    }

    fn on_ty_expr__bottom_up<Ex, En>(
        &mut self,
        elem: &mut Expr_<Ex, En>,
        _cfg: &Config,
        errs: &mut Vec<NamingPhaseError>,
    ) -> ControlFlow<(), ()>
    where
        Ex: Default,
    {
        match elem {
            Expr_::Shape(fields) => fields
                .iter_mut()
                .for_each(|(nm, _)| canonical_shape_name(nm, &self.current_class, errs)),
            _ => (),
        }
        ControlFlow::Continue(())
    }

    fn on_ty_shape_field_info_bottom_up(
        &mut self,
        elem: &mut ShapeFieldInfo,
        _cfg: &Config,
        errs: &mut Vec<NamingPhaseError>,
    ) -> ControlFlow<(), ()> {
        canonical_shape_name(&mut elem.name, &self.current_class, errs);
        ControlFlow::Continue(())
    }
}

fn canonical_shape_name(
    nm: &mut ShapeFieldName,
    current_class: &Option<String>,
    errs: &mut Vec<NamingPhaseError>,
) {
    match (nm, current_class) {
        (ShapeFieldName::SFclassConst(id, _), Some(cls_nm)) if id.name() == sn::classes::SELF => {
            id.1 = cls_nm.to_string();
        }
        (ShapeFieldName::SFclassConst(id, _), _) if id.name() == sn::classes::SELF => {
            errs.push(NamingPhaseError::Naming(NamingError::SelfOutsideClass(
                id.0.clone(),
            )));
            id.1 = sn::classes::UNKNOWN.to_string();
        }
        _ => (),
    }
}

#[cfg(test)]
mod tests {

    use oxidized::aast_defs::Hint;
    use oxidized::aast_defs::Hint_;
    use oxidized::ast_defs::Id;
    use oxidized::tast::Pos;

    use super::*;
    use crate::elab_utils;
    use crate::Transform;

    // -- Valid cases ----------------------------------------------------------

    #[test]
    fn test_shape_in_class() {
        let cfg = Config::default();
        let mut errs = Vec::default();
        let class_name = "Classy";
        let mut pass = ElabShapeFieldNamePass {
            current_class: Some(class_name.to_string()),
        };
        let mut elem: Expr_<(), ()> = Expr_::Shape(vec![(
            ShapeFieldName::SFclassConst(
                Id(Pos::default(), sn::classes::SELF.to_string()),
                (Pos::default(), String::default()),
            ),
            elab_utils::expr::null(),
        )]);
        elem.transform(&cfg, &mut errs, &mut pass);

        // Expect no errors
        assert!(errs.is_empty());

        assert!(if let Expr_::Shape(mut fields) = elem {
            let field_opt = fields.pop();
            match field_opt {
                Some((ShapeFieldName::SFclassConst(id, _), _)) => id.name() == class_name,
                _ => false,
            }
        } else {
            false
        })
    }

    #[test]
    fn test_shape_field_info_in_class() {
        let cfg = Config::default();
        let mut errs = Vec::default();
        let class_name = "Classy";
        let mut pass = ElabShapeFieldNamePass {
            current_class: Some(class_name.to_string()),
        };
        let mut elem = ShapeFieldInfo {
            optional: Default::default(),
            hint: Hint(Pos::default(), Box::new(Hint_::Herr)),
            name: ShapeFieldName::SFclassConst(
                Id(Pos::default(), sn::classes::SELF.to_string()),
                (Pos::default(), String::default()),
            ),
        };
        elem.transform(&cfg, &mut errs, &mut pass);

        // Expect no errors
        assert!(errs.is_empty());

        assert!(match elem.name {
            ShapeFieldName::SFclassConst(id, _) => id.name() == class_name,
            _ => false,
        })
    }

    // -- Invalid cases --------------------------------------------------------

    #[test]
    fn test_shape_not_in_class() {
        let cfg = Config::default();
        let mut errs = Vec::default();
        let mut pass = ElabShapeFieldNamePass::default();
        let mut elem: Expr_<(), ()> = Expr_::Shape(vec![(
            ShapeFieldName::SFclassConst(
                Id(Pos::default(), sn::classes::SELF.to_string()),
                (Pos::default(), String::default()),
            ),
            elab_utils::expr::null(),
        )]);
        elem.transform(&cfg, &mut errs, &mut pass);

        let self_outside_class_err_opt = errs.pop();
        assert!(matches!(
            self_outside_class_err_opt,
            Some(NamingPhaseError::Naming(NamingError::SelfOutsideClass(..)))
        ));

        assert!(if let Expr_::Shape(mut fields) = elem {
            let field_opt = fields.pop();
            match field_opt {
                Some((ShapeFieldName::SFclassConst(id, _), _)) => id.name() == sn::classes::UNKNOWN,
                _ => false,
            }
        } else {
            false
        })
    }

    #[test]
    fn test_shape_field_info_not_in_class() {
        let cfg = Config::default();
        let mut errs = Vec::default();
        let mut pass = ElabShapeFieldNamePass::default();
        let mut elem = ShapeFieldInfo {
            optional: Default::default(),
            hint: Hint(Pos::default(), Box::new(Hint_::Herr)),
            name: ShapeFieldName::SFclassConst(
                Id(Pos::default(), sn::classes::SELF.to_string()),
                (Pos::default(), String::default()),
            ),
        };
        elem.transform(&cfg, &mut errs, &mut pass);

        let self_outside_class_err_opt = errs.pop();
        assert!(matches!(
            self_outside_class_err_opt,
            Some(NamingPhaseError::Naming(NamingError::SelfOutsideClass(..)))
        ));

        assert!(match elem.name {
            ShapeFieldName::SFclassConst(id, _) => id.name() == sn::classes::UNKNOWN,
            _ => false,
        })
    }
}