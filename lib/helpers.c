#include "helpers.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

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
		execvp(file, argv);
		_exit(EXIT_FAILURE);
	} else {
		int status;
		waitpid(child, &status, 0);
		if (WEXITSTATUS(status) != 0)
			return -1;
		else
			return 0;
	}
}

struct execargs_t {
	char** arg;
	size_t size;
};


execargs_t* execargs_new(char* str, size_t size) {
	execargs_t* p = malloc(sizeof(execargs_t));
	if (p == NULL) {
		return NULL;
	}
	size_t i, l = 0;
	int amount = 0;
	for (i = 0; i< size; i++){
		if ((str[i] == ' ') && (i > l)) {
			l = i + 1;
			amount++;
		}
	}
	if (l < size)
		amount++;
	amount++;
	p->size = amount;
	p->arg = calloc(amount, sizeof(char*));
	if (p->arg == NULL) {
		free(p);
		return NULL;
	}
	l = 0, amount = 0;
	for (i = 0; i < size; i++){
		if ((str[i] == ' ') && (i > l)) {
			int len = i + 1 - l;
			p->arg[amount] = calloc(len, sizeof(char));
			if (p->arg[amount] == NULL) {
				int j;
				for (j = 0; j < amount; j++)
					free(p->arg[j]);
				free(p->arg);
				free(p);
				return NULL;
			}
			memcpy(p->arg[amount], str + l, len - 1);
			p->arg[amount][len] = 0;
			amount++;
		}
		if (str[i] == ' ')
			l = i + 1;
	}
	int len = size + 1 - l;
	if (l != size) {
		p->arg[amount] = calloc(len, sizeof(char));
		if (p->arg[amount] == NULL) {
			int j;
			for (j = 0; j < amount; j++)
				free(p->arg[j]);
			free(p->arg);
			free(p);
			return NULL;
		}
		memcpy(p->arg[amount], str + l, len - 1);
		amount++;
	}
	p->arg[amount] = NULL;
	return p;
}

void execargs_free(execargs_t* p) {
	size_t j;
	for (j = 0; j < p->size; j++)
		free(p->arg[j]);
	free(p->arg);
	free(p);
}

int exec(execargs_t* args) {
	execvp(args->arg[0], args->arg);
	_exit(EXIT_FAILURE);
}

void handler(int sig) {
	signal(SIGINT, SIG_IGN);
	kill(-1 * getpid(), SIGINT);
}

void handler_set() {
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = handler;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	act.sa_mask = set;
	sigaction(SIGINT, &act, 0);
}

int runpiped(execargs_t** programs, size_t n) {
	handler_set();
	fflush(stdout);

	pid_t children[n];
	int pipefd[n + 1][2];

	pipefd[0][0] = STDIN_FILENO;
	pipefd[n][1] = STDOUT_FILENO;

	size_t i;
	for (i = 1; i < n; i++) {
		if (pipe(pipefd[i]) < 0) return -1;
	}
	for (i = 0; i < n; i++) {
		int j;
		for (j = 0; j < (int)programs[i]->size; j++)
			fprintf(stderr, "%s ", programs[i]->arg[j]);
		fprintf(stderr, "\n");
		if ((children[i] = fork()) == 0) {
			if (i != 0 && close(pipefd[i][1]) < 0) {
				_exit(EXIT_FAILURE);
			}
			if (dup2(pipefd[i][0], STDIN_FILENO) < 0) _exit(EXIT_FAILURE);
			if (i != 0 && close(pipefd[i][0]) < 0) {
				_exit(EXIT_FAILURE);
			}

			if (i != n - 1 && close(pipefd[i + 1][0]) < 0) {
				_exit(EXIT_FAILURE);
			}
			if (dup2(pipefd[i + 1][1], STDOUT_FILENO) < 0) _exit(EXIT_FAILURE);
			if (i != n - 1 && close(pipefd[i + 1][1]) < 0) {
				_exit(EXIT_FAILURE);
			}

			if (exec(programs[i]) != 0) _exit(EXIT_FAILURE);

			_exit(EXIT_FAILURE);
		} else {
			if (children[i] < 0) {
				size_t j;
				for (j = 0; j < i; j++) {
					kill(children[j], SIGINT);
				}
				return -1;
			}
			if (i != 0 && (close(pipefd[i][0]) || close(pipefd[i][1]) < 0))  {
				return -1;
			}
		}
	}
	int status;
	waitpid(children[n - 1], &status, 0);
	return 0;
}
