#include "bufio.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

struct buf_t {
	size_t size;
	size_t capacity;

	char* buf;
};

typedef struct buf_t buf_t;

buf_t *buf_new(size_t capacity) {
	buf_t* p =(buf_t*) malloc(sizeof(buf_t));
	if (p == NULL) {
		return p;
	}
	p->size = 0;
	p->capacity = capacity;
	p->buf = (char*) malloc(capacity);
	if (p->buf == NULL) {
		free(p);
		return NULL;
	}
	return p;
}

void buf_free(buf_t * buf) {
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif
	free(buf->buf);
	free(buf);
}

size_t buf_capacity(buf_t *buf ) {
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif
	return buf->capacity;
}

size_t buf_size(buf_t * buf) {
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif
	return buf->size;
}

ssize_t buf_fill(int fd, buf_t *buf, size_t required) {
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif
	size_t curr_size = 0;
	ssize_t read_res;

	size_t cap = buf->capacity;
	char* curr = buf->buf;
	while (curr_size < required && curr_size < cap && (read_res = read(fd, curr + curr_size, cap - curr_size)) > 0) {
		curr_size += read_res;
	}

#ifdef DEBUG
	if (read_res > 0 && required > cap) {
		abort();
	}
#endif
	buf->size = curr_size;
	return curr_size;
}

ssize_t buf_flush(int fd, buf_t *buf, size_t required) {
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	size_t curr_size = 0;
	ssize_t write_res;
	size_t cap = buf->size;
	char* curr= buf->buf;
	
	while (curr_size < required && curr_size < cap && (write_res = write(fd, curr + curr_size, cap - curr_size)) > 0) {
		curr_size += write_res;
	}

	memmove(curr, curr + curr_size, cap - curr_size);
	
	buf->size -= curr_size;
	return curr_size;
}
