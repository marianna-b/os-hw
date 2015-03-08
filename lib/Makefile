CC = gcc

CFLAGS = -c -Wall
LDFLAGS = -shared 

RM = rm -f
TARGET_LIB = libhelpers.so

SRCS = helpers.c 
OBJS = $(SRCS:.c=.o) 

all: $(TARGET_LIB)
 
$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
 
clean:
	-$(RM) $(TARGET_LIB) $(OBJS)
