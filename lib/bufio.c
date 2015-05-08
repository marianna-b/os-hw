#include "bufio.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
	size_t curr_size = buf->size;
	ssize_t read_res;

	size_t cap = buf->capacity;
	char* curr = buf->buf;
	while (buf->size < required && buf->size < cap && (read_res = read(fd, curr + buf->size, cap - buf->size)) > 0) {
		buf->size += read_res;
	}

#ifdef DEBUG
	if (read_res > 0 && required > cap) {
		abort();
	}
#endif
	return buf->size - curr_size;
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


ssize_t get_line(fd_t fd, struct buf_t * buf, char* dest) {
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif
	
	int curr_d = 0;
	ssize_t res = 0;
	int i;

	while (1) {
		for (i = 0; i < (int)buf->size; i++) {
			if (buf->buf[i] == '\n') {

				memcpy(dest + curr_d, buf->buf, i);
				memmove(buf->buf, buf->buf + i + 1, buf->size - i - 1); 

				curr_d += i;
				buf->size -= i + 1;

				return curr_d;
			}
		}
		
		if (res < 0)
			return -1;

		memcpy(dest + curr_d, buf->buf, buf->size);
		curr_d += buf->size;
		buf->size = 0;
		
		if ((res = buf_fill(fd, buf, 1)) == 0)
			break;
	}
	return curr_d;
}

ssize_t buf_write(fd_t fd, struct buf_t *buf, char* src, size_t len) {
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	ssize_t res;
	size_t written = 0;
	size_t flushed = 0;

	while (1) {
		size_t curr = buf->capacity - buf->size;
		
		if (curr >= len) {
			memcpy(buf->buf + buf->size, src + written, len);
			buf->size += len;
			
			return len + flushed;
		} else {
			memcpy(buf->buf + buf->size, src + written, curr);
			
			buf->size += curr;
			written += curr;
			len -= curr;
			
			if ((res = buf_flush(fd, buf, buf->size)) < 0) {
				return res;
			} else if (res == 0) {
				return flushed;
			} else {
				flushed += res;
			}
		}
	}
}
