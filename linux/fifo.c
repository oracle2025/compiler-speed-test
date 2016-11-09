#include "fifo.h"

void fifo_init(fifo *f)
{
	f->start = 0;
	f->end = 0;
}

bool fifo_push(fifo *f, int value)
{
	if (fifo_is_full(f)) {
		return false;
	}
	f->buffer[f->end] = value;
	f->end = (f->end + 1) % FIFO_SIZE;
	return true;
}
int fifo_pull(fifo *f, bool *ok)
{
	if (fifo_is_empty(f)) {
		*ok = false;
		return 0;
	}
	*ok = true;
	int ret = f->buffer[f->start];
	f->start = (f->start + 1) % FIFO_SIZE;
	return ret;
}

bool fifo_is_empty(fifo *f)
{
	return f->start == f->end;
}
bool fifo_is_full(fifo *f)
{
	return ((f->end + 1) % FIFO_SIZE) == f->start;
}
int fifo_length(fifo *f)
{
	int end = f->end;
	if (end < f->start) {
		end += FIFO_SIZE;
	}
	return end - f->start;
}

