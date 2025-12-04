/* Import test framework. */
#include "gest-run.h"
#include "gest.h"

/* Import student submission. */
#include "solution.c"

/* One may use a number of different assertions:
    - assert_int_equals(a,b) to compare two integers.
    - assert_dbl_equals(a,b) to compare two floating point numbers.
    - assert_ptr_equals(a,b) to compare two pointers.
    - assert_str_equals(a,b) to compare two strings.
    - assert_that(c) to check if a condition holds. (similar to assert(c) from
   assert.h)
    - assert_true(c) synonym for assert_that.
    - assert_false(c) holds when assert_true fails and the other way around.
    - assert_fail(m) to fail with a given message.
*/

void *keep_old(void *old_data, void *new_data) { return old_data; }

void *use_new(void *old_data, void *new_data) { return new_data; }

int destroy_count = 0;

void destroy(void *data) { free(data); }
void destroy_and_count(void *data) {
  free(data);
  destroy_count++;
}

int iterate_count = 0;
int iterate_sum = 0;

void iterate_accumulate(char *key, void *data) {
  iterate_count++;
  iterate_sum += *(int *)data;
}

/* Test cases. */
void test1() {
  size_t k = 13;
  HashMap *hm = create_hashmap(k);

  assert_that(hm != NULL);

  int a = 10;
  int b = 20;

  insert_data(hm, "foo", &a, NULL);
  insert_data(hm, "bar", &b, NULL);

  int *pa = (int *)get_data(hm, "foo");
  int *pb = (int *)get_data(hm, "bar");

  assert_ptr_equals(&a, pa);
  assert_ptr_equals(&b, pb);

  assert_ptr_equals(NULL, get_data(hm, "baz"));

  delete_hashmap(hm, NULL);
}

void test2() {
  HashMap *hm = create_hashmap(3);
  assert_that(hm != NULL);

  int *v1 = malloc(sizeof(int));
  int *v2 = malloc(sizeof(int));
  int *v3 = malloc(sizeof(int));

  *v1 = 1;
  *v2 = 2;
  *v3 = 3;

  insert_data(hm, "foo", v1, NULL);
  assert_ptr_equals(v1, (int *)get_data(hm, "foo"));

  insert_data(hm, "foo", v2, keep_old);
  assert_ptr_equals(v1, (int *)get_data(hm, "foo"));

  insert_data(hm, "foo", v3, use_new);
  assert_ptr_equals(v3, (int *)get_data(hm, "foo"));

  delete_hashmap(hm, destroy);
}

void test3() {
  HashMap *hm = create_hashmap(2);
  assert_that(hm != NULL);

  int *v1 = malloc(sizeof(int));
  int *v2 = malloc(sizeof(int));

  *v1 = 1;
  *v2 = 2;

  insert_data(hm, "foo", v1, NULL);
  assert_ptr_equals(v1, (int *)get_data(hm, "foo"));
  insert_data(hm, "oof", v2, use_new);
  assert_ptr_equals(v1, (int *)get_data(hm, "foo"));
  assert_ptr_equals(v2, (int *)get_data(hm, "oof"));
}

void test4() {
  HashMap *hm = create_hashmap(17);
  assert_that(hm != NULL);

  int *v1 = malloc(sizeof(int));
  int *v2 = malloc(sizeof(int));
  int *v3 = malloc(sizeof(int));
  *v1 = 5;
  *v2 = 7;
  *v3 = 9;

  insert_data(hm, "foo", v1, NULL);
  insert_data(hm, "bar", v2, NULL);
  insert_data(hm, "baz", v3, NULL);

  iterate_count = 0;
  iterate_sum = 0;

  iterate(hm, iterate_accumulate);

  assert_int_equals(3, iterate_count);
  assert_int_equals(5 + 7 + 9, iterate_sum);

  delete_hashmap(hm, destroy);
}

void test5() {
  HashMap *hm = create_hashmap(11);
  assert_that(hm != NULL);

  destroy_count = 0;

  int *v1 = malloc(sizeof(int));
  int *v2 = malloc(sizeof(int));
  *v1 = 42;
  *v2 = 99;

  insert_data(hm, "one", v1, NULL);
  insert_data(hm, "two", v2, NULL);

  remove_data(hm, "one", destroy_and_count);
  assert_int_equals(1, destroy_count);
  assert_ptr_equals(NULL, get_data(hm, "one"));
  assert_ptr_equals(v2, get_data(hm, "two"));

  remove_data(hm, "bitch", destroy_and_count);
  assert_int_equals(1, destroy_count);

  delete_hashmap(hm, destroy_and_count);
}

/* Register all test cases. */
void register_tests() {
  register_test(test1);
  register_test(test2);
  register_test(test3);
  register_test(test4);
}
