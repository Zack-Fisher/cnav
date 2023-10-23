SRCS=$(shell find src -name '*.c' -type f) $(shell find deps/libwhisper/src -name '*.c' -type f)
OBJS=$(patsubst %.c,%.o,$(SRCS))

CC=gcc
CFLAGS=-ggdb3 -rdynamic -O0

COMPILE=$(CC) $(CFLAGS) -c
LINK=$(CC) $(CFLAGS)

all: cnav

cnav: $(OBJS)
	$(LINK) -o $@ $^

%.o: %.c
	$(COMPILE) -o $@ $< -Ideps/libwhisper/api

clean:
	rm $(shell find . -name '*.o' -type f)
