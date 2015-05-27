#ifndef HELPERS_H
#define HELPERS_H

#include <sys/types.h>

struct execargs_t;
typedef struct execargs_t execargs_t;

ssize_t write_(int, const void*, size_t);
ssize_t read_(int, const void*, size_t);
ssize_t read_until(int, void *, size_t, char);
int spawn(const char*, char* const[]);

execargs_t* execargs_new(char*, size_t);
void execargs_free(execargs_t*);

int exec(execargs_t * args);
int runpiped(execargs_t**, size_t);
void handler_set(int);

#endif
