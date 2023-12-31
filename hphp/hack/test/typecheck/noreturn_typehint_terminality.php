<?hh

function my_invariant_violation(string $arg): noreturn {
  throw new Exception($arg);
}

class C {
  public static function invariant_violation(string $arg): noreturn {
    throw new Exception($arg);
  }

  public function test_static(?int $x): int {
    if ($x === null) {
      static::invariant_violation("yup!");
    }
    return $x;
  }
}

function test_fun1(?int $x): int {
  if ($x === null) {
    my_invariant_violation("yup!");
  }
  return $x;
}

function test_fun2(?int $x): int {
  if ($x === null) {
    my_invariant_violation("yup!");
  } else {
    $y = $x;
  }
  return $y;
}

function test_meth1(?int $x): int {
  if ($x === null) {
    C::invariant_violation("yup!");
  }
  return $x;
}

function test_meth2(?int $x): int {
  if ($x === null) {
    C::invariant_violation("yup!");
  } else {
    $y = $x;
  }
  return $y;
}
