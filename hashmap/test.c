/* Import test framework. */
#include "gest.h"
#include "gest-run.h"

/* Import student submission. */
#include "solution.c"

/* Test cases. */
void test1() {
	/* One may use a number of different assertions:
     - assert_int_equals(a,b) to compare two integers.
     - assert_dbl_equals(a,b) to compare two floating point numbers.
     - assert_ptr_equals(a,b) to compare two pointers.
     - assert_str_equals(a,b) to compare two strings.
     - assert_that(c) to check if a condition holds. (similar to assert(c) from assert.h)
     - assert_fail(m) to fail with a given message.
     or cause a non-zero exit code in any other way, shape or form.
   */
}

void test2() {
}

/* Register all test cases. */
void register_tests() {
    register_test(test1);
    register_test(test2);
}
