<?hh


<<__EntryPoint>>
function main_resolve_include_curdir() :mixed{
var_dump(stream_resolve_include_path(__FILE__));
var_dump(stream_resolve_include_path(__DIR__));
var_dump(stream_resolve_include_path('/'));
}
