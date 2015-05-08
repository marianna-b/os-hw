CC = gcc

CFLAGS = -L../lib -Wall -Wl,-rpath=../lib -I../lib -g

RM = rm -f
TARGET = buffilter

SRCS = buffilter.c 
OBJS = $(SRCS:.c=.o) 
LIBS = -lhelpers -lbufio

all: $(TARGET)
 
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
 
clean:
	-$(RM) $(TARGET) $(OBJS)
