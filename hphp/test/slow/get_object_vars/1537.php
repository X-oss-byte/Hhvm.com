<?hh

class Base{
  public    $aaa = 1;
  protected $bbb = 2;
  private   $ccc = 3;
}
class Child extends Base{
  public    $ddd = 4;
  protected $eee = 5;
  private   $fff = 6;
  function foo($obj) :mixed{
    var_dump(get_class($obj));
    var_dump(get_object_vars($obj));
  }
}

<<__EntryPoint>>
function main_1537() :mixed{
$child_obj = new Child();
$base_obj = new Base();
$child_obj->foo($base_obj);
}
