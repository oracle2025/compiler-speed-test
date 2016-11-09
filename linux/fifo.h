/* fifo.h */

#ifndef _FIFO_H_

#define _FIFO_H_

#include <stdbool.h>

#define FIFO_SIZE 10

typedef struct fifo_t {
	int buffer[FIFO_SIZE];
	int start;
	int end;
} fifo;

void fifo_init(fifo *f);

bool fifo_push(fifo *f, int value);
int fifo_pull(fifo *f, bool *ok);

bool fifo_is_empty(fifo *f);
bool fifo_is_full(fifo *f);

int fifo_length(fifo *f);


#endif /* end of include guard: _FIFO_H_ */
