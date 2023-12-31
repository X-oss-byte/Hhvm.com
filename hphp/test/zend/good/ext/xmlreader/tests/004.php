<?hh
/* $Id$ */
<<__EntryPoint>> function main(): void {
$filename = sys_get_temp_dir().'/'.'_004.xml';
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book num="1" idx="2">book1</book></books>';
file_put_contents($filename, $xmlstring);

$reader = new XMLReader();
if (!$reader->open($filename)) {
    exit();
}

while ($reader->read()) {
    if ($reader->nodeType != XMLReader::END_ELEMENT) {
        echo $reader->name."\n";
        if ($reader->nodeType == XMLReader::ELEMENT && $reader->hasAttributes) {
            $attr = $reader->moveToFirstAttribute();
            while ($attr) {
                echo "   Attribute Name: ".$reader->name."\n";
                echo "   Attribute Value: ".$reader->value."\n";
                $attr = $reader->moveToNextAttribute();
            }
        }
    }
}
$reader->close();
unlink($filename);
echo "===DONE===\n";
}
