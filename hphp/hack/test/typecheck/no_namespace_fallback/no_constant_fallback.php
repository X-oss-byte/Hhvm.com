<?hh
/**
 * Copyright (c) 2014, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *
 */

namespace {
  const string FOO = 'bar';
}

namespace NS {
  function bar(): void {
    \var_dump(FOO);
  }
}
