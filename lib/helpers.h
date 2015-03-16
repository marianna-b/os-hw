#ifndef HELPERS_H
#define HELPERS_H

#include <sys/types.h>

ssize_t write_(int, const void*, size_t);
ssize_t read_(int, const void*, size_t);
ssize_t read_until(int, void *, size_t, char);

#endif
