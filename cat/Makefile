CC = gcc

CFLAGS = -L../lib -Wall -Wl,-rpath=../lib

RM = rm -f
TARGET = cat

SRCS = cat.c 
OBJS = $(SRCS:.c=.o) 
LIBS = -lhelpers

all: $(TARGET)
 
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
 
clean:
	-$(RM) $(TARGET) $(OBJS)
