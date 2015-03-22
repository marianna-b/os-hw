#include "helpers.h"

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

ssize_t write_(int fd, const void* buf, size_t count) {
	size_t idx = 0;
	ssize_t res = 0;
	
	while (idx < count && (res = write(fd, buf + idx, count - idx)) > 0) {
		idx += res;
		res = 0;
	}
	return res;
}

ssize_t read_(int fd, const void* buf, size_t count) {
	size_t idx = 0;
	ssize_t res = 0;
	
	while (idx < count && (res = read(fd, (char*)buf + idx, count - idx)) > 0) {
		idx += res;
	}
	
	if (res < 0) {
		return res;
	} else {
		return idx;
	}
}

ssize_t read_until(int fd, void * buf, size_t count, char delimiter) {
	size_t idx = 0;
	ssize_t res = 0;
	
	while (idx < count && (res = read(fd, (char*)buf + idx, count - idx)) > 0) {
		int i;
		for (i = 0; i < res; i++) {
			if (((char*)buf)[idx + i] == delimiter) {
				return idx + res;
			}
			
		}
		idx += res;
	}
	
	if (res < 0) {
		return res;
	} else {
		return idx;
	}
}

int spawn(const char* file, char* const argv[]) {
	pid_t child;
	if ((child = fork()) == 0) {
		return execvp(file, argv);
	} else {
		int status;
		if (waitpid(child, &status, 0) < 0)
			return -1;
		else
			return 0;
	}
}
