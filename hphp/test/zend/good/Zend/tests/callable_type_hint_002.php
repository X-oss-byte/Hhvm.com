<?hh

class bar {
    static function foo(callable $arg) :mixed{}
}
function foo(callable $bar) :mixed{
}
<<__EntryPoint>> function main(): void {
$closure = function (callable $arg) {};
$rf = new ReflectionFunction("foo");
var_dump($rf->getParameters()[0]->isCallable());

$rm = new ReflectionMethod("bar", "foo");
var_dump($rm->getParameters()[0]->isCallable());

$rc = new ReflectionFunction($closure);
var_dump($rc->getParameters()[0]->isCallable());
}
