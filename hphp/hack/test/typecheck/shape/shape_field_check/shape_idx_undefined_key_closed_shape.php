<?hh

function test(shape('x' => int) $s): void {
  Shapes::idx($s, 'y');
}
