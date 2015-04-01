all: libbufio.so libhelpers.so

MAKE = @make --no-print-directory

BUFIO_SRC = bufio.c
MAKE_BUFIO = $(MAKE) SRCS=$(BUFIO_SRC) NAME=bufio -f mklib.mk

HELPERS_SRC = helpers.c
MAKE_HELPERS = $(MAKE) SRCS=$(HELPERS_SRC) NAME=helpers -f mklib.mk

libbufio.so:
	$(MAKE_BUFIO)

libhelpers.so:
	$(MAKE_HELPERS)

clean:
	$(MAKE_BUFIO) clean
	$(MAKE_HELPERS) clean

