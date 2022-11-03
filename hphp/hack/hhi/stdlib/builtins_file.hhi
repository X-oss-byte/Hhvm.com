<?hh /* -*- php -*- */
/**
 * Copyright (c) 2014, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 */

/* HH_FIXME[4110] */
const resource STDIN = 0;
/* HH_FIXME[4110] */
const resource STDOUT = 0;
/* HH_FIXME[4110] */
const resource STDERR = 0;

const int PATHINFO_DIRNAME = 0;
const int PATHINFO_BASENAME = 0;
const int PATHINFO_EXTENSION = 0;
const int PATHINFO_FILENAME = 0;
const string DIRECTORY_SEPARATOR = "/";
const string PATH_SEPARATOR = ":";

const int UPLOAD_ERR_OK = 0;
const int UPLOAD_ERR_INI_SIZE = 1;
const int UPLOAD_ERR_FORM_SIZE = 2;
const int UPLOAD_ERR_PARTIAL = 3;
const int UPLOAD_ERR_NO_FILE = 4;
const int UPLOAD_ERR_NO_TMP_DIR = 6;
const int UPLOAD_ERR_CANT_WRITE = 7;
const int UPLOAD_ERR_EXTENSION = 8;

const int FILE_APPEND = 0;
const int FILE_USE_INCLUDE_PATH = 0;
const int FILE_IGNORE_NEW_LINES = 0;
const int FILE_NO_DEFAULT_CONTEXT = 0;
const int FILE_SKIP_EMPTY_LINES = 0;

const int LOCK_EX = 2;
const int LOCK_SH = 1;
const int LOCK_UN = 3;
const int LOCK_NB = 4;

const int SEEK_SET = 0;
const int SEEK_CUR = 0;
const int SEEK_END = 0;

const int INI_SCANNER_NORMAL = 0;
const int INT_SCANNER_RAW = 0;

const int GLOB_ERR     = 1;
const int GLOB_MARK    = 2;
const int GLOB_NOSORT  = 4;
const int GLOB_NOCHECK = 16;
const int GLOB_ESCAPE  = 64;
const int GLOB_BRACE   = 1024;
const int GLOB_ONLYDIR = 8192;

<<__PHPStdLib>>
function fopen(
  string $filename,
  string $mode,
  bool $use_include_path = false,
  $context = null,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function popen(string $command, string $mode): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fclose(resource $handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function pclose($handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fseek(
  resource $handle,
  int $offset,
  int $whence = SEEK_SET,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function rewind(resource $handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function ftell(resource $handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function feof(resource $handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fstat(resource $handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fread(resource $handle, int $length): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fgetc(resource $handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fgets(
  resource $handle,
  int $length = 0,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fgetss(
  resource $handle,
  int $length = 0,
  string $allowable_tags = "",
): HH\FIXME\MISSING_RETURN_TYPE;
function fscanf(
  resource $handle,
  string $format,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fpassthru(resource $handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fwrite(
  resource $handle,
  string $data,
  int $length = 0,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fputs(
  resource $handle,
  string $data,
  int $length = 0,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fprintf(
  resource $handle,
  string $format,
  ...$args
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function vfprintf($handle, $format, $args): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fflush(resource $handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function ftruncate(resource $handle, int $size): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function flock(
  resource $handle,
  int $operation,
  inout $wouldblock,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fputcsv(
  resource $handle,
  $fields,
  string $delimiter = ",",
  string $enclosure = "\"",
  string $escape_char = "\\",
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fgetcsv(
  resource $handle,
  int $length = 0,
  string $delimiter = ",",
  string $enclosure = "\"",
  string $escape_char = "\\",
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function file_get_contents(
  string $filename,
  bool $use_include_path = false,
  $context = null,
  int $offset = 0,
  int $maxlen = 0,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function file_put_contents(
  string $filename,
  $data,
  int $flags = 0,
  $context = null,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function file(
  string $filename,
  int $flags = 0,
  $context = null,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function readfile(
  string $filename,
  bool $use_include_path = false,
  $context = null,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function move_uploaded_file(
  string $filename,
  string $destination,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function parse_ini_file(
  string $filename,
  bool $process_sections = false,
  int $scanner_mode = INI_SCANNER_NORMAL,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function parse_ini_string(
  string $ini,
  bool $process_sections = false,
  int $scanner_mode = INI_SCANNER_NORMAL,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function md5_file(
  string $filename,
  bool $raw_output = false,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function sha1_file(
  string $filename,
  bool $raw_output = false,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function chmod(string $filename, int $mode): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function chown(string $filename, $user): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function lchown(string $filename, $user): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function chgrp(string $filename, $group): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function lchgrp(string $filename, $group): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function touch(
  string $filename,
  int $mtime = 0,
  int $atime = 0,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function copy(
  string $source,
  string $dest,
  $context = null,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function rename(
  string $oldname,
  string $newname,
  $context = null,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function umask($mask = null): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function unlink(
  string $filename,
  $context = null,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function link(string $target, string $link): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function symlink(string $target, string $link): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function basename(
  string $path,
  string $suffix = "",
)[]: HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fnmatch(
  string $pattern,
  string $filename,
  int $flags = 0,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function glob(string $pattern, int $flags = 0): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function tempnam(string $dir, string $prefix): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function tmpfile(): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fileperms(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fileinode(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function filesize(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fileowner(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function filegroup(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function fileatime(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function filemtime(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function filectime(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function filetype(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function linkinfo(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function is_writable(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function is_writeable(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function is_readable(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function is_executable(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function is_file(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function is_dir(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function is_link(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function is_uploaded_file(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function file_exists(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function stat(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function lstat(string $filename): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function clearstatcache(): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function readlink(string $path): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function realpath(string $path)[read_globals]: HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function pathinfo(string $path, int $opt = 15): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function disk_free_space(string $directory): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function diskfreespace(string $directory): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function disk_total_space(string $directory): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function mkdir(
  string $pathname,
  int $mode = 0777,
  bool $recursive = false,
  $context = null,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function rmdir(string $dirname, $context = null): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function dirname(string $path)[]: HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function getcwd(): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function chdir(string $directory): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function chroot(string $directory): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function dir(string $directory): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function opendir(string $path, $context = null): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function readdir($dir_handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function rewinddir($dir_handle): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function scandir(
  string $directory,
  bool $descending = false,
  $context = null,
): HH\FIXME\MISSING_RETURN_TYPE;
<<__PHPStdLib>>
function closedir($dir_handle): HH\FIXME\MISSING_RETURN_TYPE;
