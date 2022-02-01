// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the "hack" directory of this source tree.

use intern::path::PathId;
use std::path::PathBuf;

pub use oxidized::relative_path::Prefix;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct RelativePath {
    prefix: Prefix,
    suffix: PathId,
}

impl RelativePath {
    pub fn new(prefix: Prefix, suffix: PathId) -> Self {
        Self { prefix, suffix }
    }

    pub fn to_absolute(&self, ctx: &RelativePathCtx) -> PathBuf {
        let mut buf = match self.prefix {
            Prefix::Root => &ctx.root,
            Prefix::Hhi => &ctx.hhi,
            Prefix::Tmp => &ctx.tmp,
            Prefix::Dummy => &ctx.dummy,
        }
        .to_owned();
        self.suffix.push_to(&mut buf);
        buf
    }
}

#[derive(Debug, Clone)]
pub struct RelativePathCtx {
    pub root: PathBuf,
    pub hhi: PathBuf,
    pub tmp: PathBuf,
    pub dummy: PathBuf,
}
