CC = gcc

CFLAGS = -c -Wall -fpic
LDFLAGS = -shared 

RM = rm -f
TARGET_LIB = lib$(NAME).so

OBJS = $(SRCS:.c=.o) 

all: $(TARGET_LIB)
 
$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
 
clean:
	-$(RM) $(TARGET_LIB) $(OBJS)
