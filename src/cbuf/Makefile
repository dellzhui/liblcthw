
CC = gcc
CFLAGS = -g -Wall
target = test

SRCS = main.c porting_linux.c
OBJS = $(SRCS:.c=.o)

all : $(target)

$(target) : $(OBJS)
	$(CC) -o $@ $(OBJS) -lpthread

%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -rf *.o test