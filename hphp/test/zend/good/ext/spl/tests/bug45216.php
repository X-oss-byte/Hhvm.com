<?hh
<<__EntryPoint>> function main(): void {
$file = sys_get_temp_dir().'/'.'foo.html';
file_put_contents($file, 'text 0<div class="tested">text 1</div>');
$handle = fopen($file, 'r');

$object = new SplFileObject($file);
var_dump($object->fgetss());
var_dump(fgetss($handle));

unlink($file);
}
