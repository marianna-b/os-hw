CC = gcc
CFLAGS = -Wall -I../lib
LDFLAGS = -L../lib -Wl,-rpath=../lib
LDLIBS = -lhelpers -lbufio

RM = rm -f
TARGET = polling forking
OBJS = polling.o forking.o

all: $(TARGET)

polling: polling.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

forking: forking.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
 
clean:
	-$(RM)  $(TARGET) $(OBJS)
