// @generated by hh_manual from manual/hack/11-built-in-types/35-enum-class.md
// @codegen-command : buck run fbcode//hphp/hack/src/hh_manual:hh_manual extract fbcode/hphp/hack/manual/hack/
enum class Foo: string {
  string BAR = 'BAZ';
}

function do_stuff(HH\MemberOf<Foo, string> $value): void {
  var_dump($value);
}

function main(): void {
  do_stuff(Foo::BAR); // ok
}
