<?hh
function start_elem($parser,$name,$attribs) :mixed{
    print "$name ";

    foreach($attribs as $key => $value) {
        print "$key = $value ";
    }
    print "\n";
}
function end_elem()
:mixed{
}
<<__EntryPoint>> function main(): void {
$xml = <<<HERE
<a xmlns="http://example.com/foo"
    xmlns:bar="http://example.com/bar">
  <bar:b foo="bar"/>
  <bar:c bar:nix="null" foo="bar"/>
</a>
HERE;

$parser = xml_parser_create_ns("ISO-8859-1","@");
xml_set_element_handler($parser,start_elem<>,end_elem<>);
xml_parser_set_option($parser,XML_OPTION_CASE_FOLDING,0);
xml_parse($parser, $xml);
xml_parser_free($parser);
}
