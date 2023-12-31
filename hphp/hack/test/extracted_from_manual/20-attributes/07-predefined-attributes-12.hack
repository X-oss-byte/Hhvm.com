// @generated by hh_manual from manual/hack/20-attributes/07-predefined-attributes.md
// @codegen-command : buck run fbcode//hphp/hack/src/hh_manual:hh_manual extract fbcode/hphp/hack/manual/hack/
class Button {
  public function draw(): void { /* ... */ }
}

trait MyButtonTrait {
  <<__Override>>
  public function draw(): void { /* ... */ }
}

class ExampleButton extends Button {
  // If ExampleButton did not have an inherited method
  // called 'draw', this would be an error.
  use MyButtonTrait;
}
