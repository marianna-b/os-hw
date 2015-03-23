#include "../lib/helpers.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const int BUF_SIZE = 4096;

ssize_t write_stdout(void* buf, size_t l) {
	if (write_(STDOUT_FILENO, buf, l) < 0) {
		perror("Can't write to stdout");
		return -1;
    }
	return 0;
}

int main() {
        char buf[BUF_SIZE];
		char tmp_buf[BUF_SIZE];
        ssize_t read_res;
        int idx = 0;
        
        while ((read_res = read_until(STDIN_FILENO, buf + idx, BUF_SIZE - idx, ' ')) >= 0) {
	        if (read_res == 0) {
		        if (idx > 0) {
			        sprintf(tmp_buf, "%d", idx);
			        if (write_stdout(tmp_buf, strlen(tmp_buf)) < 0) goto ERROR;
		        }
		        break;
	        } else {
		        int i, l = 0, n = read_res + idx;
			    for (i = 0; i < n; i++) {
				    if (buf[i] == ' ') {
					    sprintf(tmp_buf, "%d ", i - l);
					    if (write_stdout(tmp_buf, strlen(tmp_buf)) < 0) goto ERROR;
					    l = i + 1;
				    }
			    }
			    if (l == 0 && n == BUF_SIZE) {
				    sprintf(tmp_buf, "%d ", BUF_SIZE);
				    if (write_stdout(tmp_buf, strlen(tmp_buf)) < 0) goto ERROR;
			        idx = 0;
			    } else {
				    if (l == n - 1) {
					    idx = 0;
					} else {
					    idx = n - l;
					    memmove(buf, buf + l, idx);
				    }
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
