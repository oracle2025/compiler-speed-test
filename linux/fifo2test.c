#include <stdio.h>
#include "minunit.h"
#include "fifo2.h"

int tests_run = 0;

static char * test_fifo_init() {
	fifo_t myfifo;
	fifo_t* f = &myfifo;
	const int size = 128;
	char buffer[size];
	fifo_init(f, buffer, size);
	mu_assert("f->head == 0", f->head == 0);
	mu_assert("f->tail == 0", f->tail == 0);
	return 0;
}
static char * test_fifo_empty() {
	fifo_t myfifo;
	fifo_t* f = &myfifo;
	const int size = 128;
	char buffer[size];
	fifo_init(f, buffer, size);
	mu_assert("1: f->head == f->tail", f->head == f->tail);
	unsigned int value = 15;
	fifo_write(f, &value, sizeof(unsigned int));
	fifo_read(f, &value, sizeof(unsigned int));
	mu_assert("2: f->head == f->tail", f->head == f->tail);
	return 0;
}
static char * test_fifo_full() {
	fifo_t myfifo;
	fifo_t* f = &myfifo;
	const int size = 128;
	char buffer[size];
	fifo_init(f, buffer, size);
	unsigned int value = 15;
	for (int i = 0; i < (128/4)-1; i++) {
		size_t written = fifo_write(f, &value, sizeof(value));
		mu_assert("written == sizeof(value)", written == sizeof(value));
	}
	size_t written = fifo_write(f, &value, sizeof(value));
	written = fifo_write(f, &value, sizeof(value));
	mu_assert("written == 0", written == 0);
	return 0;
}
static char * test_fifo_overflow() {
	fifo_t myfifo;
	fifo_t* f = &myfifo;
	const int size = 128;
	char buffer[size];
	fifo_init(f, buffer, size);
	unsigned int value = 15;
	for (int i = 0; i < (128/4)-1; i++) {
		mu_assert("fifo_write", fifo_write(f, &value, sizeof(value)) == sizeof(value));
	}
	for (int i = 0; i < (128/4)/2; i++) {
		mu_assert("fifo_read", fifo_read(f, &value, sizeof(value)) == sizeof(value));
	}

	for (int i = 0; i < (128/4)/2; i++) {
		mu_assert("fifo_write", fifo_write(f, &value, sizeof(value)) == sizeof(value));
	}
	for (int i = 0; i < (128/4)-1; i++) {
		mu_assert("fifo_read", fifo_read(f, &value, sizeof(value)) == sizeof(value));
	}
	mu_assert("f->head == f->tail", f->head == f->tail);
	return 0;
}
static char * all_tests() {
	mu_run_test(test_fifo_init);
	mu_run_test(test_fifo_empty);
	mu_run_test(test_fifo_full);
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
