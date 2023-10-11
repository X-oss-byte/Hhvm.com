//// file1.php
<?hh

interface Constraint<T as num> {}

newtype NewConstraint<T as num> as Constraint<T> = Constraint<T>;

//// file2.php
<?hh

// Test that we check constraints for the 'as' part of newtypes
function invalid(NewConstraint<mixed> $x): void {}
