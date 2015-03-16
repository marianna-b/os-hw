#include "../lib/helpers.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const size_t BUF_SIZE = 4096;

ssize_t write_stdout(void* buf, size_t l) {
	if (write_(STDOUT_FILENO, buf, l) < 0) {
		perror("Can't write to stdout");
		return -1;
    }
	return 0;
}

void reverse(char* buf, size_t l, size_t r) {
	size_t i, j;
	size_t n = (r - l);
	size_t half = n / 2;
	for (i = 0; i < half; i++) {
		j = n - 1 - i;
		buf[l + i] = buf[l + i] ^ buf[l + j];
		buf[l + j] = buf[l + i] ^ buf[l + j];
		buf[l + i] = buf[l + i] ^ buf[l + j];
	}
}

int main() {
        char buf[BUF_SIZE];
        ssize_t read_res;
        size_t idx = 0;
        
        while ((read_res = read_until(STDIN_FILENO, buf + idx, BUF_SIZE - idx, ' ')) >= 0) {
	        if (read_res == 0) {
				reverse(buf, 0, idx);
				if (write_stdout(buf, idx) < 0) goto ERROR;
		        break;
	        } else {
		        size_t i, l = 0, n = read_res + idx;
			    for (i = 0; i < n; i++) {
				    if (buf[i] == ' ') {
					    reverse(buf, l, i);
					    if (write_stdout(buf + l, i - l + 1) < 0) goto ERROR;
					    l = i + 1;
				    }
			    }
			    if (l == 0 && n == BUF_SIZE) {
				    reverse(buf, 0, BUF_SIZE);
			        if (write_stdout(buf, BUF_SIZE) < 0) goto ERROR;
			        idx = 0;
			    } else {
				    idx = n - l;
				    memmove(buf, buf + l, idx);
			    }
		    }
        }

        if (read_res < 0) {
                perror("Can't read from stdin");
                goto ERROR;
        }

        return 0;

ERROR:
        return EXIT_FAILURE;
}
