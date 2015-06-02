CC = gcc
CFLAGS = -Wall -I../lib
LDFLAGS = -L../lib -Wl,-rpath=../lib
LDLIBS = -lhelpers -lbufio

RM = rm -f
TARGET = filesender

SRCS = filesender.c 
OBJS = $(SRCS:.c=.o) 

all: $(TARGET)

$(TARGET): $(OBJS)
 
clean:
	-$(RM) $(TARGET) $(OBJS)
