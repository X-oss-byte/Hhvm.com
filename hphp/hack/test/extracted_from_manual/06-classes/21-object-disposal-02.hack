// @generated by hh_manual from manual/hack/06-classes/21-object-disposal.md
// @codegen-command : buck run fbcode//hphp/hack/src/hh_manual:hh_manual extract fbcode/hphp/hack/manual/hack/
class Example implements IAsyncDisposable {
  public async function __disposeAsync(): Awaitable<void> {
    // Cleanup here.
  }
}
