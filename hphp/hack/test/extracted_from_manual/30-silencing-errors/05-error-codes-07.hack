// @generated by hh_manual from manual/hack/30-silencing-errors/05-error-codes.md
// @codegen-command : buck run fbcode//hphp/hack/src/hh_manual:hh_manual extract fbcode/hphp/hack/manual/hack/
function foo(): shape('x' => int) {
  /* HH_FIXME[4057] Missing the field `x`. */
  return shape();
}
