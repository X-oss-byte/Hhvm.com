// @generated by hh_manual from manual/hack/15-asynchronous-operations/28-guidelines.md
// @codegen-command : buck run fbcode//hphp/hack/src/hh_manual:hh_manual extract fbcode/hphp/hack/manual/hack/
function get_hello(): string {
  return "Hello";
}

<<__EntryPoint>>
function run_na_hello(): void {
  \var_dump(get_hello());
}
