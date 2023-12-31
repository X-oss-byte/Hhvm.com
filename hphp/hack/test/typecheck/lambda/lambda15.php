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

type F<T> = (function(): T);
class Foo<T> {
  public function __construct(private T $x) {}
  private function get(): T {
    return $this->x;
  }
  public function getter(): F<T> {
    return () ==> $this->get();
  }
  public function thisser(): (function(): this) {
    return () ==> $this;
  }
}

function doubley(float $x): void {}

function x(): void {
  $x = new Foo(2);
  $x = $x->thisser();
  $x = $x();
  $x = $x->getter();
  doubley($x());
}
