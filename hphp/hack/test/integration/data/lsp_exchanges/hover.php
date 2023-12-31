<?hh
// comment
function a_hover(): int {
  return b_hover();
}

// A comment describing b_hover.
function b_hover(): int {
  return 42;
}

// A comment describing THE_ANSWER
const int THE_ANSWER = 42;

function return_the_answer(): int {
  return THE_ANSWER;
}

// comment describing the "pad_left" function and its parameters
function pad_left(string $s, int $i, string $pad): string {
  return $s;
}

// comment describing "return_a_string" function
function return_a_string(): string {
  $pad = "hello";
  $x = pad_left("StringToPad", 20, $pad);
  return $x;
}

// Copyright 2004-present Facebook. All Rights Reserved.
// Testing copyright removal
final class CopyrightClass {
  public static function copyrightMethod(): void {}
}

function testing_copyright_autocomplete(): void {
  CopyrightClass::copyrightMethod();
}

/**
 * This file is generated. Do not modify it manually!
 *
 * This file was generated by:
 *   scripts/xcontroller/gencode XIndiaFreeFBPetitionController
 * If there is a merge conflict, run
 *   scripts/xcontroller/gencode XIndiaFreeFBPetitionController --merge
 *
 * For codegen logic, refer to XControllerCodegenScript.
 *
 * @generated SignedSource<<foo>>
 */
// Testing generated text removal
final class GeneratedClass {
  public static function generatedMethod(): void {}
}

function testing_generated_autocomplete(): void {
  GeneratedClass::generatedMethod();
}

function test_xhp_attribute(): void {
  <xhp:enum-attribute name="abc" enum-attribute={MyEnum::TYPE_A} />;
  <xhp:generic id={EntSomething::getId()} />;
}

// An empty space to add text with textDocument/didChange notifications
// for testing hover on incomplete/uncompilable hack.
function testing_area_hover(): void {

}
