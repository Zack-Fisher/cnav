SRCS=$(shell find src -name '*.c' -type f) $(shell find deps/libwhisper/src -name '*.c' -type f)
OBJS=$(patsubst %.c,%.o,$(SRCS))

CC=gcc
CFLAGS=-ggdb3 -rdynamic -O0

COMPILE=$(CC) $(CFLAGS) -c
LINK=$(CC) $(CFLAGS)

INSTALL_PATH=/bin/cnav

all: cnav

install: cnav
	ln -sf "$(shell pwd)/cnav" $(INSTALL_PATH)

chsh: 
	echo $(INSTALL_PATH) >> /etc/shells
	chsh -s $(INSTALL_PATH) $(USER)

cnav: $(OBJS)
	$(LINK) -o $@ $^

%.o: %.c
	$(COMPILE) -o $@ $< -Ideps/libwhisper/api

clean:
	rm $(shell find . -name '*.o' -type f)

.PHONY: clean chsh install cnav all
