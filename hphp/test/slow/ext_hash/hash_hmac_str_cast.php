<?hh
class Stringy {
  public function __toString()[] :mixed{ return "hello"; }
}

<<__EntryPoint>>
function main_hash_hmac_str_cast() :mixed{
$key = "c0793dfd477a2ff11b7cd5636594a76b518f3a6df8fb450d90ac52457833604e";
$hello_hash = hash_hmac("sha256", "hello", $key, false);
$one_hash = hash_hmac("sha256", "1", $key, false);
var_dump( $hello_hash === hash_hmac("sha256", new Stringy(), $key, false) );
var_dump( $one_hash === hash_hmac("sha256", 1, $key, false) );
}
