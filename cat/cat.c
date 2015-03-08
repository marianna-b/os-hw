#include "helpers.h"
#include ".h"

int main() {
	char buf[1024];
	int read_res, write_res;
	while ((read_res = read_(STDIN_FILENO, buf, 1024)) > 0) {
		if ((write_res = write_(STDOUT_FILENO, buf, read_res)) < 0) {
			perror("Can't write to stdout");
			goto ERROR
		}
	}
	if (read_res < 0) {
		perror("Can't read from stdin");
		goto ERROR
	}
	return 0;

ERROR:
	return EXIT_FAILURE;
}
