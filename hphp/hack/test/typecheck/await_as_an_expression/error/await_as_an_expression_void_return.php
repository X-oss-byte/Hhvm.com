<?hh

async function genx(): Awaitable<void> {}

async function foo(): Awaitable<void> {
  $x = await genx();
}
