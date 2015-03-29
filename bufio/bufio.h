#ifndef BUFIO_H
#define BUFIO_H

#include <sys/types.h>

struct buf_t;

typedef struct buf_t buf_t;
typedef int fd_t;

struct buf_t *buf_new(size_t);

void buf_free(struct buf_t *);

size_t buf_capacity(struct buf_t *);

size_t buf_size(struct buf_t *);

ssize_t buf_fill(fd_t, struct buf_t *, size_t);

ssize_t buf_flush(fd_t, struct buf_t *, size_t);

#endif
