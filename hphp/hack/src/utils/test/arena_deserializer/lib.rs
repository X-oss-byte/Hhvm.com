// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the "hack" directory of this source tree.

#![cfg(test)]

use arena_deserializer::*;
use bumpalo::Bump;
use serde::{Deserialize, Serialize};
use serde_json;

fn round_trip<'a, X: Deserialize<'a> + Serialize + Eq + std::fmt::Debug>(x: X, arena: &'a Bump) {
    let se = serde_json::to_string(&x).unwrap();
    let mut de = serde_json::Deserializer::from_str(&se);
    let de = ArenaDeserializer::new(arena, &mut de);
    let x2 = X::deserialize(de).unwrap();
    assert_eq!(x, x2);
}

#[test]
fn example() {
    #[derive(Serialize, Deserialize, Debug, PartialEq, Eq)]
    struct I(isize);

    #[derive(Serialize, Deserialize, Debug, PartialEq, Eq)]
    enum Num<'a> {
        #[serde(deserialize_with = "arena_deserializer::arena")]
        Base(&'a I),
        #[serde(deserialize_with = "arena_deserializer::arena")]
        Succ(&'a Num<'a>),
        #[serde(deserialize_with = "arena_deserializer::arena")]
        List(&'a [&'a Num<'a>]),
        #[serde(deserialize_with = "arena_deserializer::arena")]
        ListValue(&'a [Num<'a>]),
        #[serde(deserialize_with = "arena_deserializer::arena")]
        Str(&'a str),
        String(String),
        #[serde(deserialize_with = "arena_deserializer::arena")]
        OptRef(Option<&'a isize>),
        #[serde(deserialize_with = "arena_deserializer::arena")]
        Opt(Option<isize>),
        #[serde(deserialize_with = "arena_deserializer::arena")]
        PairRef(&'a (bool, &'a isize)),
        #[serde(deserialize_with = "arena_deserializer::arena")]
        Pair((&'a Num<'a>, &'a isize)),
    }

    deserialize_without_arena!(Num<'arena>, I);

    let arena = Bump::new();

    let x = I(0);
    round_trip(x, &arena);

    let i = Num::Str("aa");
    let x = Num::Succ(&i);
    round_trip(x, &arena);

    let i = I(3);
    let x = Num::Base(&i);
    round_trip(x, &arena);

    let i = I(1);
    let n1 = Num::Base(&i);
    let n2 = Num::Succ(&n1);
    let ls = vec![&n1, &n2];
    let x = Num::List(&ls);
    round_trip(x, &arena);

    let i = I(1);
    let n1 = Num::Base(&i);
    let n2 = Num::Succ(&n1);
    let n1 = Num::Base(&i);
    let ls = vec![n1, n2];
    let x = Num::ListValue(&ls);
    round_trip(x, &arena);

    let i: isize = 3;
    let x = Num::OptRef(Some(&i));
    round_trip(x, &arena);

    let i: isize = 3;
    let x = Num::Opt(Some(i));
    round_trip(x, &arena);

    let i: isize = 1;
    let n = Num::Opt(Some(3));
    let p = (&n, &i);
    let x = Num::Pair(p);
    round_trip(x, &arena);
}
