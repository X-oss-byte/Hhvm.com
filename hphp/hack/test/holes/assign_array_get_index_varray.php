<?hh

function assign_array_get_index_varray(varray<int> $xs, mixed $idx): void {
  /* HH_FIXME[4324] */
  $xs[$idx] = 1;
}
