<?hh

class MyException extends Exception {
  function __construct($_errno, $_errmsg) {
    $this->errno = $_errno;
    $this->errmsg = $_errmsg;
  }

  function getErrno() :mixed{
    return $this->errno;
  }

  function getErrmsg() :mixed{
    return $this->errmsg;
  }
}

function ErrorsToExceptions($errno, $errmsg) :mixed{
  throw new MyException($errno, $errmsg);
}

<<__EntryPoint>>
function main() :mixed{
  set_error_handler(ErrorsToExceptions<>);

  // make sure it isn't catching exceptions that weren't
  // thrown...

  try {
  } catch (MyException $exception) {
    echo "There was an exception: " . $exception->getErrno() . ", '" . $exception->getErrmsg() . "'\n";
  }

  try {
    trigger_error("I will become an exception", E_USER_ERROR);
  } catch (MyException $exception) {
    echo "There was an exception: " . $exception->getErrno() . ", '" . $exception->getErrmsg() . "'\n";
  }
}
