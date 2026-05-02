CC = gcc
CFLAGS = -Wall -g
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OBJS_BUILD = $(wildcard .*.o)

all: main

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

main: $(OBJS) $(OBJS_BUILD)
	@$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS) main

.PHONY: all clean main
