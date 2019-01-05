#include <stdio.h>
#include "minunit.h"
#include "fifo.h"

 int tests_run = 0;

 static char * test_fifo_init() {
	 fifo f1;
	 fifo_init(&f1);
	 mu_assert("f1.start == 0", f1.start == 0);
	 mu_assert("f1.end == 0", f1.end == 0);
	 return 0;
 }

static char * test_fifo_empty() {
	 fifo myfifo;
	 fifo *f1 = &myfifo;
	 fifo_init(f1);
	 mu_assert("init;fifo_is_empty(f1)", fifo_is_empty(f1));
	 fifo_push(f1, 5);
	 bool ok;
	 fifo_pull(f1, &ok);
	 mu_assert("push;pull;fifo_is_empty(f1)", fifo_is_empty(f1));
	 return 0;
}

static char * test_fifo_push() {
	 fifo myfifo;
	 fifo *f1 = &myfifo;
	 fifo_init(f1);
	 mu_assert("fifo_push(f1, 5)", fifo_push(f1, 5));
	 mu_assert("fifo_length(f1) == 1", fifo_length(f1) == 1);
	 return 0;
}

static char * test_fifo_full() {
	fifo myfifo;
	fifo *f1 = &myfifo;
	fifo_init(f1);
	for (int i = 0; i < FIFO_SIZE-1; i++) {
		mu_assert("fifo_push(f1, x)", fifo_push(f1, i+10));
	}
	mu_assert("full;fifo_push(f1, x)", fifo_push(f1, 10) == false);
	mu_assert("fifo_is_full(f1)", fifo_is_full(f1));
	return 0;
}

static char * test_fill_fifo_empty_fifo() {
	fifo myfifo;
	fifo *f1 = &myfifo;
	fifo_init(f1);
	for (int i = 0; i < FIFO_SIZE-1; i++) {
		mu_assert("fifo_push(f1, x)", fifo_push(f1, i+10));
	}
	mu_assert("fifo_is_full(f1)", fifo_is_full(f1));
	for (int i = 0; i < FIFO_SIZE-1; i++) {
		bool ok;
		int value;
		value = fifo_pull(f1, &ok);
		mu_assert("ok", ok);
		mu_assert("value == i+10", value == i+10);
	}
	mu_assert("fifo_is_empty(f1)", fifo_is_empty(f1));
	return 0;
}

static char * test_fifo_overflow() {
	fifo myfifo;
	fifo *f1 = &myfifo;
	fifo_init(f1);
	for (int i = 0; i < FIFO_SIZE-1; i++) {
		mu_assert("fifo_push(f1, x)", fifo_push(f1, i+10));
	}
	mu_assert("fifo_is_full(f1)", fifo_is_full(f1));

	for (int i = 0; i < FIFO_SIZE/2; i++) {
		bool ok;
		int value;
		value = fifo_pull(f1, &ok);
		mu_assert("ok", ok);
	}
	mu_assert("fifo_length(f1) == FIFO_SIZE-1 - (FIFO_SIZE/2)", fifo_length(f1) == FIFO_SIZE-1 - (FIFO_SIZE/2));
	for (int i = 0; i < FIFO_SIZE/2; i++) {
		mu_assert("fifo_push(f1, x)", fifo_push(f1, i+10));
	}
	mu_assert("fifo_is_full(f1)", fifo_is_full(f1));
	for (int i = 0; i < FIFO_SIZE-1; i++) {
		bool ok;
		int value;
		value = fifo_pull(f1, &ok);
		mu_assert("ok", ok);
	}
	mu_assert("fifo_is_empty(f1)", fifo_is_empty(f1));
	mu_assert("f1->start == FIFO_SIZE/2-1", f1->start == FIFO_SIZE/2-1);
	mu_assert("f1->end == FIFO_SIZE/2-1", f1->end == FIFO_SIZE/2-1);
	
	return 0;
}
 

static char * all_tests() {
	mu_run_test(test_fifo_init);
	mu_run_test(test_fifo_empty);
	mu_run_test(test_fifo_push);
	mu_run_test(test_fifo_full);
	mu_run_test(test_fill_fifo_empty_fifo);
	mu_run_test(test_fifo_overflow);
	return 0;
}

 int main(int argc, char **argv) {
     char *result = all_tests();
     if (result != 0) {
         printf("%s\n", result);
     }
     else {
         printf("ALL TESTS PASSED\n");
     }
     printf("Tests run: %d\n", tests_run);
 
     return result != 0;
 }

