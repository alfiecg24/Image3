.PHONY: all dirs clean
CC = gcc
IMAGE3_CFLAGS = -Iinclude -Iexternal/libDER/include
IMAGE3_SOURCES = $(wildcard src/*.c) $(wildcard external/libDER/src/*.c)
IMAGE3_TESTS = $(patsubst tests/%, %, $(wildcard tests/*))
IMAGE3_OUTPUT_DIR = build

all: dirs $(IMAGE3_TESTS)

$(IMAGE3_TESTS): %: tests/%/main.c $(IMAGE3_SOURCES)
	$(CC) $(IMAGE3_CFLAGS) -o $(IMAGE3_OUTPUT_DIR)/$@ $^

dirs:
	@mkdir -p $(IMAGE3_OUTPUT_DIR)

clean:
	rm -rf $(IMAGE3_OUTPUT_DIR)