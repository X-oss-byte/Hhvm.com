<?hh

async function genEager($a) :Awaitable<mixed>{
  try {
    echo "eager execution\n";
    return 10;
  } finally {
    echo "finally\n";
  }
}

function block() :mixed{ // simulates blocking I/O
  return RescheduleWaitHandle::create(
    RescheduleWaitHandle::QUEUE_NO_PENDING_IO,
    1,
  );
}
async function genBlocking($a) :Awaitable<mixed>{
  try {
    echo "before blocking\n";
    await block();
    echo "after blocking\n";
    return 10;
  } finally {
    echo "finally\n";
  }
}

function main() :mixed{
  echo "* eager async *\n";
  $result = HH\Asio\join(genEager(42));
  var_dump($result);

  echo "* blocking async *\n";
  $result = HH\Asio\join(genBlocking(42));
  var_dump($result);
}

<<__EntryPoint>>
function main_async() :mixed{
;
main();
}
