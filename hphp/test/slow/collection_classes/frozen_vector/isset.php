<?hh

// Test isset() with ImmVector.

function main() :mixed{
  $fv = ImmVector {1, 2, 3};
  var_dump(isset($fv[0]));
  var_dump(isset($fv[2]));
  var_dump(isset($fv[3]));
}


<<__EntryPoint>>
function main_isset() :mixed{
main();
}
