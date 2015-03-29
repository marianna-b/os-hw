CC = gcc

CFLAGS = -L../bufio -Wall -Wl,-rpath=../bufio  -I../bufio

RM = rm -f
TARGET = bufcat

SRCS = bufcat.c 
OBJS = $(SRCS:.c=.o) 
LIBS = -lbufio

all: $(TARGET)
 
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
 
clean:
	-$(RM) $(TARGET) $(OBJS)
