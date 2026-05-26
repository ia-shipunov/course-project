CC = gcc
CFLAGS = -std=c99 -Wall -O2 -lm
IDIR = include
LDIR = libs

LIB_SRC = src/image.c src/filters.c
LIB_OBJ = $(LIB_SRC:.c=.o)

TEST_SRC = tests/test_main.c tests/test_filters.c
TEST_OBJ = $(TEST_SRC:.c=.o)

all: imgproc

imgproc: src/main.c $(LIB_OBJ)
	$(CC) $(CFLAGS) -I$(IDIR) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I$(IDIR) -c $< -o $@

test: $(TEST_OBJ) $(LIB_OBJ)
	$(CC) $(CFLAGS) -I$(IDIR) -o test_runner $^
	./test_runner

clean:
	rm -f imgproc test_runner src/*.o tests/*.o