<?hh


<<__EntryPoint>>
function main_808() :mixed{
$m = new Map;
$m[3] = 'foo';
$m['bar'] = 7;
var_dump(serialize($m));
$m2 = unserialize(serialize($m));
echo "------------------------\n";
var_dump($m);
print_r($m);
echo json_encode($m) . "\n";
var_export($m);
 echo "\n";
var_dump($m2);
print_r($m2);
echo json_encode($m2) . "\n";
var_export($m2);
 echo "\n";
echo "------------------------\n";
$m = new Map;
$m['bar'] = 7;
$m[3] = 'foo';
var_dump(serialize($m));
$m2 = unserialize(serialize($m));
echo "------------------------\n";
var_dump($m);
print_r($m);
echo json_encode($m) . "\n";
var_export($m);
 echo "\n";
var_dump($m2);
print_r($m2);
echo json_encode($m2) . "\n";
var_export($m2);
 echo "\n";
}
