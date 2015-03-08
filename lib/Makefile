CC = gcc

CFLAGS = -c -Wall
LDFLAGS = -shared -o

RM = rm -f
TARGET_LIB = libhelpers.so

SRCS = helpers.c 
OBJS = $(SRCS:.c=.o) 

all: ${TARGET_LIB}
 
$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} $(TARGET_LIB) $(OBJS)
 
$(SRCS:.c=.o): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS)
 
clean:
	-${RM} ${TARGET_LIB} ${OBJS}
