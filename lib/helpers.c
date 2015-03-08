#include "helpers.h"

#include <unistd.h>
#include <string.h>

ssize_t write_(int fd, const void* buf, size_t count) {
	size_t idx = 0;
	ssize_t res = 0;
	
	while (idx < count && (res = write(fd, buf + idx, count - idx)) > 0) {
		idx += res;
		res = 0;
	}
	return res;
}

ssize_t read_(int fd, const void* buf) {
	size_t idx = 0;
	ssize_t count = 0;
	char curr_buf[128];
	
	while ((count = read(fd, curr_buf, 128)) > 0) {
		memcpy((char *) buf + idx, curr_buf, count);
		idx += count;
	}
	
	if (count == 0) {
		return idx;
	} else {
		return count;
	}
}
