<?hh

class A {
  public $a = 2;
  public $b = 1;
}
class B extends A {
  public $b = 3;
}

<<__EntryPoint>>
function main_650() :mixed{
$obj = new A();
 var_dump($obj);
 var_dump($obj->b);
$obj = new B();
 var_dump($obj);
 var_dump($obj->b);
}
