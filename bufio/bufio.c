#include "bufio.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
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
	
}

size_t buf_capacity(buf_t *buf ) {
	return 0;
}

size_t buf_size(buf_t * buf) {
	return 0;
}

ssize_t buf_fill(int fd, buf_t *buf, size_t required) {
	return -1;
}

ssize_t buf_flush(int fd, buf_t *buf, size_t required) {
	return -1;
}
