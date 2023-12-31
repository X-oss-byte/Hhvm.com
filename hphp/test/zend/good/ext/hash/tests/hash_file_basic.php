<?hh
/* Prototype  : string hash_file(string algo, string filename[, bool raw_output = false])
 * Description: Generate a hash of a given file
 * Source code: ext/hash/hash.c
 * Alias to functions: 
 */
<<__EntryPoint>> function main(): void {
echo "*** Testing hash_file() : basic functionality ***\n";

// Set up file
$filename = sys_get_temp_dir().'/'.'hash_file_basic_example.txt';
file_put_contents( $filename, 'The quick brown fox jumped over the lazy dog.' );

var_dump( hash_file( 'md5', $filename ) );
var_dump( hash_file( 'sha1', $filename ) );
var_dump( hash_file( 'sha256', $filename ) );
var_dump( hash_file( 'sha512', $filename ) );

var_dump( base64_encode( hash_file( 'md5', $filename, true ) ) );

echo "===DONE===\n";

unlink( $filename );
}
