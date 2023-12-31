// @generated by hh_manual from manual/hack/11-built-in-types/13-num.md
// @codegen-command : buck run fbcode//hphp/hack/src/hh_manual:hh_manual extract fbcode/hphp/hack/manual/hack/
class Point {
  private float $x;
  private float $y;
  public function __construct(num $x = 0, num $y = 0) {
    $this->x = (float)$x;
    $this->y = (float)$y;
  }
  public function move(num $x = 0, num $y = 0): void {
    $this->x = (float)$x;
    $this->y = (float)$y;
  }
  // ...
}
