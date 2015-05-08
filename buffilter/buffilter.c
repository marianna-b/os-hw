#include "helpers.h"
#include "bufio.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const size_t BUF_SIZE = 9;
const char delimiter = '\n';


int main(int arg_count, char* argv[]) {
        char buf[BUF_SIZE];
        ssize_t read_res;
        char* args[arg_count + 1];
        
        if (arg_count < 2) goto ERROR;
        
        int i;
        for (i = 0; i < arg_count - 1; i ++) {
	        args[i] = argv[i + 1];
        }

        args[arg_count] = NULL;
        buf_t* buffer_in = buf_new(BUF_SIZE);
        buf_t* buffer_out = buf_new(BUF_SIZE);
        
        while ((read_res = get_line(STDIN_FILENO, buffer_in, buf)) >= 0) {
	        if (read_res == 0) {
		        break;
	        } else {
					buf[read_res] = 0;
				    args[arg_count - 1] = buf;
				    if (spawn(argv[1], args) == 0) {
					    buf[read_res] = '\n';
					    buf[read_res + 1] = 0;
					    if (buf_write(STDOUT_FILENO, buffer_out, buf, read_res + 1) < 0) goto ERROR;
				    }
				    
		    }
        }

        if (read_res < 0) {
	        perror("Can't read from stdin");
            goto ERROR;
        }
        size_t s = buf_size(buffer_out);
        if (buf_flush(STDOUT_FILENO, buffer_out, s) < 0) goto ERROR;
	        
	    buf_free(buffer_in);
	    buf_free(buffer_out);
        return 0;

ERROR:
	    buf_free(buffer_in);
	    buf_free(buffer_out);
        return EXIT_FAILURE;
}
