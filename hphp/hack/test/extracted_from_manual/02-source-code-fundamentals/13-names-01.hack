// @generated by hh_manual from manual/hack/02-source-code-fundamentals/13-names.md
// @codegen-command : buck run fbcode//hphp/hack/src/hh_manual:hh_manual extract fbcode/hphp/hack/manual/hack/
class Data {
  const int MAX_VALUE = 100;
  private int $value = 0;
  /* ... */
}
interface ICollection { /* ... */ }
enum Position: int {
  Top = 0;
  Bottom = 1;
  Left = 2;
  Right = 3;
  Center = 4;
}
function compute(int $val): void {
  $count = $val + 1;
  /* ... */
}
