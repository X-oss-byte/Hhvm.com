<?hh

function reflectMethod($class, $method) :mixed{
    $methodInfo = new ReflectionMethod($class, $method);
    echo "**********************************\n";
    echo "Reflecting on method $class::$method()\n\n";
    echo "\nisFinal():\n";
    var_dump($methodInfo->isFinal());
    echo "\nisAbstract():\n";
    var_dump($methodInfo->isAbstract());
    echo "\nisPublic():\n";
    var_dump($methodInfo->isPublic());
    echo "\nisPrivate():\n";
    var_dump($methodInfo->isPrivate());
    echo "\nisProtected():\n";
    var_dump($methodInfo->isProtected());
    echo "\nisStatic():\n";
    var_dump($methodInfo->isStatic());
    echo "\nisConstructor():\n";
    var_dump($methodInfo->isConstructor());
    echo "\n**********************************\n";
}

class TestClass
{
    public function foo() :mixed{
        echo "Called foo()\n";
    }

    static function stat() :mixed{
        echo "Called stat()\n";
    }

    private function priv() :mixed{
        echo "Called priv()\n";
    }

    protected function prot() :mixed{}
}

class DerivedClass extends TestClass {}

interface TestInterface {
    public function int():mixed;
}
<<__EntryPoint>> function main(): void {
reflectMethod("DerivedClass", "foo");
reflectMethod("TestClass", "stat");
reflectMethod("TestClass", "priv");
reflectMethod("TestClass", "prot");
reflectMethod("DerivedClass", "prot");
reflectMethod("TestInterface", "int");
reflectMethod("ReflectionProperty", "__construct");
}
