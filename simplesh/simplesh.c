#include <helpers.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

const size_t BUF_SIZE = 4096;
const char delimiter = '\n';
const char pipechar = '|';
const char* str = "$";

int main() {
	char buf[BUF_SIZE];

	execargs_t* progs[BUF_SIZE];
	handler_set(1);

	if (write_(STDIN_FILENO, str, 1) < 0) goto ERROR;
	
	int amount = 0, idx = 0, readres;
	while ((readres = read_until(STDIN_FILENO, buf + idx, BUF_SIZE - idx, delimiter)) != 0) {
		if (readres < 0) {
			if (errno == EINTR) 
				continue;
			else 
				break;
		}
		size_t p = 0, n = readres + idx;
		int i;

		for (i = 0; i < (int)n; i++) {
			if ((buf[i] == pipechar) || buf[i] == delimiter) {
				if (p != i) {
					if ((progs[amount] = execargs_new(buf + p, i - p)) == NULL) goto ERROR;
					amount++;
				}
				p = i + 1;
				if (buf[i] == delimiter) {
					if (runpiped(progs, amount) < 0) goto ERROR;

					int j;
					for (j = 0; j < amount; j++) {
						execargs_free(progs[j]);
					}

					amount = 0;
					if (write_(STDIN_FILENO, str, 1) < 0) goto ERROR;
				}
			}
		}
		memmove(buf, buf + p, readres - p);
		idx = readres - p;
	}
	int j;
	for (j = 0; j < amount; j++) {
		execargs_free(progs[j]);
	}
	if (readres < 0) goto ERROR;
	
	return 0;
ERROR:
	perror("");
	return 1;
}
