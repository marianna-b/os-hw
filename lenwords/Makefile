CC = gcc

CFLAGS = -L../lib -Wall -Wl,-rpath=../lib -g

RM = rm -f
TARGET = lenwords

SRCS = lenwords.c 
OBJS = $(SRCS:.c=.o) 
LIBS = -lhelpers

all: $(TARGET)
 
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
 
clean:
	-$(RM) $(TARGET) $(OBJS)
