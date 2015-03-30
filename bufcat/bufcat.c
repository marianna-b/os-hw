#include "bufio.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

const size_t BUF_SIZE = 4096;

int main() {
	buf_t* buf = buf_new(BUF_SIZE);
	while (1) {
		ssize_t read_res = buf_fill(STDIN_FILENO, buf, BUF_SIZE);
		if (buf_flush(STDOUT_FILENO, buf, BUF_SIZE) < 0) goto ERROR;
		if (read_res <= 0) goto ERROR;
	}
	buf_free(buf);
	return 0;
ERROR:
	buf_free(buf);
	return EXIT_FAILURE;
}
