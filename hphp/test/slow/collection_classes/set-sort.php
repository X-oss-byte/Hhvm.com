<?hh
function main() :mixed{
  for ($i = 0; $i < 8; ++$i) {
    if ($i < 4) {
      $s = Set {3, 4, 1, 2};
    } else {
      $s = Set {'b', 'a', 'd', 'c'};
    }
    var_dump($s);
    switch ($i % 4) {
      case 0: asort(inout $s); break;
      case 1: ksort(inout $s); break;
      case 2:
        uasort(inout $s,
          function ($l, $r) { return $l < $r ? -1 : ($l === $r ? 0 : 1); });
        break;
      case 3:
        uksort(inout $s,
          function ($l, $r) { return $l < $r ? -1 : ($l === $r ? 0 : 1); });
        break;
    }
    var_dump($s);
    unset($s);
    echo "----\n";
  }
}

<<__EntryPoint>>
function main_set_sort() :mixed{
main();
}
