#include "bufio.h"
#include <stdlib.h>
#include <unistd.h>

const size_t BUF_SIZE = 4096;

int main() {
	buf_t* buf = buf_new(BUF_SIZE);
	ssize_t read_res;
	while (true) {
		read_res = buf_fill(STDIN_FILENO, buf, BUF_SIZE);
		if (buf_flush(STDOUT_FILENO, buf, BUF_SIZE) < 0) goto ERROR;
		if (read_res <= 0) goto ERROR;
		buf_free(buf);
		buf = buf_new(BUF_SIZE);
	}
	return 0;

ERROR:
	perror();
	return EXIT_FAILURE;
}
