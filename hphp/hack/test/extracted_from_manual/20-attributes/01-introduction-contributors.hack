// @generated by hh_manual from manual/hack/20-attributes/01-introduction.md
// @codegen-command : buck run fbcode//hphp/hack/src/hh_manual:hh_manual extract fbcode/hphp/hack/manual/hack/
class Contributors implements HH\ClassAttribute {
  public function __construct(private string $author, private ?keyset<string> $maintainers = null) {}
  public function getAuthor(): string {
    return $this->author;
  }
  public function getMaintainers(): keyset<string> {
    return $this->maintainers ?? keyset[$this->author];
  }
}

<<Contributors("John Doe", keyset["ORM Team", "Core Library Team"])>>
class MyClass {}

<<Contributors("You")>>
class YourClass {}

async function example_snippet_wrapper1(): Awaitable<void> {
  $rc = new ReflectionClass('MyClass');
  $my_class_contributors = $rc->getAttributeClass(Contributors::class);
  $my_class_contributors?->getAuthor(); // "John Doe"
  $my_class_contributors?->getMaintainers(); // keyset["ORM Team", "Core Library Team"]
}
