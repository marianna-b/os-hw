CC = gcc

CFLAGS = -L../lib -Wall -Wl,-rpath=../lib  -I../lib

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
