<?hh
// Copyright 2004-present Facebook. All Rights Reserved.

class C<T as arraykey> {
  public function __construct(private T $item) {}
  public static function id(T $x): T {
    return $x;
  }
  public static function selfy(C<T> $c): T {
    return $c->item;
  }
}
function test(int $x): string {
  $z = C::id(true);
  return C::selfy(new C(false));
}
