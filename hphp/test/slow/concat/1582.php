<?hh


<<__EntryPoint>>
function main_1582() :mixed{
$a = "1";
$a .= "2";
$a .= "3";
$a .= "4";
var_dump($a);
$a .= "1";
$a .= "2";
$a .= "3";
$a .= "4";
var_dump($a);
}
