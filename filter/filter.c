#include "../lib/helpers.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const size_t BUF_SIZE = 4096;
const char delimiter = '\n';


int main(int arg_count, char* argv[]) {
        char buf[BUF_SIZE];
        char tmp_buf[BUF_SIZE];
        ssize_t read_res;
        size_t idx = 0;
        char* args[arg_count];
        int i;
        if (arg_count < 2) goto ERROR;
        
        for (i = 0; i < arg_count - 1; i ++) {
	        args[i] = argv[i + 1];
        }
        
        while ((read_res = read_until(STDIN_FILENO, buf + idx, BUF_SIZE - idx, delimiter)) >= 0) {
	        if (read_res == 0) {
		        break;
	        } else {
		        size_t l = 0, n = read_res + idx;
		        for (i = 0; i < (int)n; i++){
			        if (buf[i] != delimiter) {
				        continue;
			        }
				    memset(tmp_buf, 0, sizeof(tmp_buf));
				    size_t bound = l == 0 ? 0 : l + 1;
				    memcpy(tmp_buf, buf + bound, i - bound);

				    args[arg_count - 1] = tmp_buf;

				    if (spawn(argv[1], args) == 0) {
					    tmp_buf[i - bound] = delimiter;
					    if (write_(STDOUT_FILENO, tmp_buf, i - bound + 1) < 0) goto ERROR;
				    }
				    l = i;
		        }
		        
				size_t bound = l == 0 ? 0 : l + 1;
		        memmove(buf, buf + bound, n - bound);
		        idx = n - bound;
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
