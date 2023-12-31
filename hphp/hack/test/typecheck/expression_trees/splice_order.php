<?hh

<<file: __EnableUnstableFeatures('expression_trees')>>

function test(
  ExprTree<ExampleDsl, ExampleDsl::TAst, ExampleFunction<(function(int, string): void)>> $foo,
  ExprTree<ExampleDsl, ExampleDsl::TAst, ExampleFunction<(function(float, bool): int)>> $bar,
  ExprTree<ExampleDsl, ExampleDsl::TAst, ExampleFunction<(function(): float)>> $baz,
  ExprTree<ExampleDsl, ExampleDsl::TAst, ExampleFunction<(function(): bool)>> $qux,
  ExprTree<ExampleDsl, ExampleDsl::TAst, ExampleFunction<(function(): string)>> $qaal,
): void {
  ExampleDsl`${$foo}(${$bar}(${$baz}(), ${$qux}()), ${$qaal}())`;
}
