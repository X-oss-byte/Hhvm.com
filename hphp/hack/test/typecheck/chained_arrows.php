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

class A {
  public function foo(): this { return $this; }
  public function bar(): int { return 0; }
}

function test(): bool {
  return (new A())->foo()->bar();
}
