<?hh

<<__EntryPoint>>
function entrypoint_get_defined_constants_basic(): void {
  /* Prototype  : array get_defined_constants  ([ bool $categorize  ] )
   * Description:  Returns an associative array with the names of all the constants and their values
   * Source code: Zend/zend_builtin_functions.c
   */

  echo "*** Testing get_defined_constants() : basic functionality ***\n";

  var_dump(gettype(get_defined_constants(true)));
  var_dump(gettype(get_defined_constants()));

  $arr1 = get_defined_constants(false);
  $arr2 = get_defined_constants();
  var_dump(array_udiff($arr1, $arr2, ($left, $right) ==> $left === $right));

  $n1 = count(get_defined_constants());

  require_once(__DIR__.'/get_defined_constants_basic.inc');

  $arr2 = get_defined_constants();
  $n2 = count($arr2);

  if ($n2 == $n1 + 1 && array_key_exists("USER_CONSTANT", $arr2)) {
  	echo "TEST PASSED\n";
  } else {
  	echo "TEST FAILED\n";
  }

  echo "===DONE===\n";
}
