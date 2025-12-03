#ifndef GEST_H
#define GEST_H

#define register_test(f) register_test_internal(f, #f)
#define assert_int_equals(x, y) assert_int_equals_internal(__FILE__, __extension__ __FUNCTION__, __LINE__, #x, x, #y, y)
#define assert_dbl_equals(x, y) assert_dbl_equals_internal(__FILE__, __extension__ __FUNCTION__, __LINE__, #x, x, #y, y)
#define assert_str_equals(x, y) assert_str_equals_internal(__FILE__, __extension__ __FUNCTION__, __LINE__, #x, x, #y, y)
#define assert_ptr_equals(x, y) assert_ptr_equals_internal(__FILE__, __extension__ __FUNCTION__, __LINE__, #x, x, #y, y)
#define assert_mem_equals(x, y, l) assert_mem_equals_internal(__FILE__, __extension__ __FUNCTION__, __LINE__, #x, x, #y, y, l)
#define assert_that(x) assert_that_internal(__FILE__, __extension__ __FUNCTION__, __LINE__, #x, x)
#define assert_true(x) assert_that(x)
#define assert_false(x) assert_that(!(x))
#define assert_fail(x) assert_fail_internal(__FILE__, __extension__ __FUNCTION__, __LINE__, x)

typedef void (*test)();

void register_test_internal(test ptr, char *name);
void unregister_tests();
void run_suite();
void show_results();
void assert_int_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, int x, const char *yname, int y);
void assert_dbl_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, double x, const char *yname, double y);
void assert_ptr_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, void *x, const char *yname, void *y);
void assert_str_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, char *x, const char *yname, char *y);
void assert_mem_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, void *x, const char *yname, void *y, int length);
void assert_that_internal(const char *file_name, const char *function_name, int line_number, const char *condition_name, int condition);
void assert_fail_internal(const char *file_name, const char *function_name, int line_number, char *message);

#endif // GEST_H
