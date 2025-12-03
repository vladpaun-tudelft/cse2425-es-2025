#include "gest.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <unistd.h> 

#define ASSERTION_FAILURE_EXITCODE 47

typedef struct test_case {
    test ptr;
    char *name;
    int passed;
    struct test_case *next;
} test_case;

test_case *test_cases = NULL;

void register_test_internal(test ptr, char *name) {
    test_case *tc = malloc(sizeof(test_case));
    tc->ptr = ptr;
    tc->name = strdup(name);
    tc->passed = -1;
    tc->next = test_cases;
    test_cases = tc;
}

void unregister_tests() {
    while (test_cases != NULL) {
        test_case *cur = test_cases;
        test_cases = cur->next;
        free(cur->name);
        free(cur);
    }
}

void run_test_case(test_case *tc) {
	printf("Starting test case: '%s'\n", tc->name);
    #ifndef SEQUENTIAL
	fflush(stdout);		// such that Valgrind does not print first
	pid_t pid = fork();
	
	if (pid == 0) {
		(tc->ptr)();
		unregister_tests();	// Clean up to avoid Valgrind nagging
		exit(0);
	}
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		tc-> passed = 1;
	}
	else {
		tc->passed = 0;
	}
    #else
    tc->ptr();
    tc->passed = 1;
    #endif
    
}

void run_suite() {
	test_case *tc = test_cases;
    while (tc != NULL) {
		run_test_case(tc);
		tc = tc->next;
	}
}

void show_results() {
    int total_score = 0;
    int achieved_score = 0;
    
    for (test_case *cur = test_cases; cur != NULL; cur = cur->next) {
        total_score++;
        if (cur->passed <= 0) {
            printf("Test case failed: '%s'\n", cur->name);
        } else {
            achieved_score++;
        }
    }
    
	printf("Tests passed: %d/%d\n", achieved_score, total_score);
}

void assert_int_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, int x, const char *yname, int y) {
    if (x != y) {
        fprintf(stderr, "%s:%s:%d: Expected <%s>:%d to be equal to <%s>:%d.\n", file_name, function_name, line_number, xname, x, yname, y);
        exit(ASSERTION_FAILURE_EXITCODE);
    }
}

void assert_dbl_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, double x, const char *yname, double y) {
    if (x != y) {
        fprintf(stderr, "%s:%s:%d: Expected <%s>:%f to be equal to <%s>:%f.\n", file_name, function_name, line_number, xname, x, yname, y);
        exit(ASSERTION_FAILURE_EXITCODE);
    }
}

void assert_ptr_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, void *x, const char *yname, void *y) {
    if (x != y) {
        fprintf(stderr, "%s:%s:%d: Expected <%s>:%p to be equal to <%s>:%p.\n", file_name, function_name, line_number, xname, x, yname, y);
        exit(ASSERTION_FAILURE_EXITCODE);
    }
}

void assert_str_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, char *x, const char *yname, char *y) {
    if (strcmp(x, y) != 0) {
        fprintf(stderr, "%s:%s:%d: Expected <%s>:\"%s\" to be equal to <%s>:\"%s\".\n", file_name, function_name, line_number, xname, x, yname, y);
        exit(ASSERTION_FAILURE_EXITCODE);
    }
}

void assert_that_internal(const char *file_name, const char *function_name, int line_number, const char *condition_name, int condition) {
    if (!condition) {
        fprintf(stderr, "%s:%s:%d: Expected '%s' to hold.\n", file_name, function_name, line_number, condition_name);
        exit(ASSERTION_FAILURE_EXITCODE);
    }
}

void assert_mem_equals_internal(const char *file_name, const char *function_name, int line_number, const char *xname, void *x, const char *yname, void *y, int length) {
    if (memcmp(x, y, length) != 0) {
        fprintf(stderr, "%s:%s:%d: Expected memory at <%s>:%p to be equal to <%s>:%p.\n", file_name, function_name, line_number, xname, x, yname, y);
        exit(ASSERTION_FAILURE_EXITCODE);
    }
}

void assert_fail_internal(const char *file_name, const char *function_name, int line_number, char *message) {
    fprintf(stderr, "%s:%s:%d: %s.\n", file_name, function_name, line_number, message);
    exit(ASSERTION_FAILURE_EXITCODE);
}
