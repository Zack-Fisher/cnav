SRCS=$(shell find src -name '*.c' -type f) $(shell find deps/libwhisper/src -name '*.c' -type f)
OBJS=$(patsubst %.c,%.o,$(SRCS))

CC=gcc
CFLAGS=-ggdb3 -rdynamic -O0

GDB=gf2

COMPILE=$(CC) $(CFLAGS) -c
LINK=$(CC) $(CFLAGS)

INSTALL_PATH=/bin/cnav

all: cnav

test: cnav_test
	./cnav_test

test_debug: cnav_test
	$(GDB) -ex run ./cnav_test

install: cnav
	ln -sf "$(shell pwd)/cnav" $(INSTALL_PATH)

chsh: 
	echo $(INSTALL_PATH) >> /etc/shells
	chsh -s $(INSTALL_PATH) $(USER)

cnav: $(OBJS)
	$(LINK) -o $@ $^

# concat the test and normal srcs, and remove the typical entrypoint.
TEST_SRCS=$(SRCS) $(shell find test -name '*.c' -type f)
TEST_SRCS := $(filter-out src/main.c, $(TEST_SRCS))
TEST_OBJS=$(patsubst %.c,%.o,$(TEST_SRCS))

$(info $(TEST_SRCS))
$(info $(TEST_OBJS))

cnav_test: $(TEST_OBJS)
	$(LINK) -o $@ $^

%.o: %.c
	$(COMPILE) -o $@ $< -Ideps/libwhisper/api

clean:
	rm $(shell find . -name '*.o' -type f)

dummy:

.PHONY: clean chsh install cnav all dummy test test_debug
