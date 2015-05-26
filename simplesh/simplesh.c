#include "helpers.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

const size_t BUF_SIZE = 4096;
const char delimiter = '\n';
const char pipechar = '|';
const char* str = "$ ";

int main() {
	char buf[BUF_SIZE];
	execargs_t* progs[BUF_SIZE];
	if (write_(STDIN_FILENO, str, 2) < 0) goto ERROR;
	int amount = 0, idx = 0, readres;
	
	while ((readres = read_until(STDIN_FILENO, buf + idx, BUF_SIZE - idx, delimiter)) > 0) {
		size_t p = 0, n = readres + idx;
		int i;
		for (i = 0; i < (int)n; i++) {
			//fprintf(stderr, "%d ", i);
			if ((buf[i] == pipechar) || buf[i] == delimiter) {
				//fprintf(stderr, "%d %d\n", p, i);
				if (p != i) {
					//fprintf(stderr, "LOL");
					if ((progs[amount] = execargs_new(buf + p, i - p)) == NULL) goto ERROR;
					//fprintf(stderr, "LOL2");
					amount++;
				}
				p = i + 1;
				if (buf[i] == delimiter) {
					fprintf(stderr, "%d\n", amount);
					if (runpiped(progs, amount) < 0) goto ERROR;
					amount = 0;
					if (write_(STDIN_FILENO, str, 2) < 0) goto ERROR;
				}
			}
		}
		memmove(buf, buf + p, readres - p);
		idx = readres - p;
	}

	if (readres < 0) {
		perror("Can't read from stdin");
		goto ERROR;
	}
	return 0;
ERROR:
	return 1;
}
