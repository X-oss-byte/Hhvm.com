<?hh <<__EntryPoint>> function main(): void {
$file = sys_get_temp_dir().'/'.'im.wbmp';

$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);
imagesetpixel($im, 3,3, 0x0);
imagewbmp($im, $file);

$im2 = imagecreatefromwbmp($file);
echo 'test create wbmp: ';
$c = imagecolorsforindex($im2, imagecolorat($im2, 3,3));
$failed = false;
foreach ($c as $v) {
    if ($v != 0) {
        $failed = true;
    }
}
echo !$failed ? 'ok' : 'failed';
echo "\n";
unlink($file);
}
