CC := gcc
CFLAGS += -Isrc -Iapi -Wall -g 

# Directories
SRC_DIR := src 
OBJ_DIR := build
TEST_SRC_DIR := test_src

TEST_TARGET := whispertest
TARGET := libwhisper.a

# just compile everything in SRC_DIR.
SRCS := $(shell find $(SRC_DIR) -type f -name "*.c") 
TEST_SRCS := $(shell find $(TEST_SRC_DIR) -type f -name "*.c") 
OBJS = $(SRCS:.c=.o)

all: notify clean $(TARGET)
	@echo  Successfully created the library at $(TARGET).

notify: 
	@echo STARTING UP LIBWHISPER BUILD...

test: clean $(TEST_TARGET)
	@echo  Successfully created the test at $(TEST_TARGET). Running tests...
	./$(TEST_TARGET)

# then archive them into a .a file.
$(TARGET): $(OBJS) 
	ar rcs $(TARGET) $(OBJS)

# putting test.c outside of the SRC_DIR, so it won't be picked up in the library build.
# link the test.c file against the library, and just run the executable generated to test the library.
# link test.c BEFORE the .a static lib?? why does this matter??
$(TEST_TARGET): $(TARGET)
	$(CC) -static $(TEST_SRCS) $< -o $@ $(CFLAGS) 

# compile all the objects
.c.o:
	$(CC) $(CFLAGS) -c $<  -o $@


clean:
	rm -f $(shell find . -name "*.o") $(TARGET) $(TEST_TARGET)
