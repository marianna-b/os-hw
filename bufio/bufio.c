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
	buf_t* p = malloc(sizeof(buf_t));
	if (p == NULL) {
		return p;
	}
	p->size = 0;
	p->capacity = capacity;
	p->buf = (char*) malloc(capacity);
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
	char* curr_buf = buf->buf;
	while (cap != curr_size && (read_res = read(fd, curr_buf + curr_size, cap - curr_size)) > 0) {
		curr_size += read_res;
	}

#ifdef DEBUG
	if (read_res > 0 && required > cap) {
		if (buf == NULL) {
			abort();
		}
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
	size_t cap = buf->capacity;
	char* curr_buf = buf->buf;
	
	while (cap != curr_size && (write_res = write(fd, curr_buf + curr_size, cap - curr_size)) > 0) {
		curr_size += write_res;
	}

	memmove(curr_buf, curr_buf + curr_size, cap - curr_size);
	
	buf->size -= curr_size;
	return curr_size;
}
