CC ?= cc
CFLAGS ?= -std=c11 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE -Wall -Wextra -O2

all: railway

railway: backend/server.c backend/railway.c backend/railway.h
	$(CC) $(CFLAGS) backend/server.c backend/railway.c -o $@

run: railway
	./railway

test_railway: backend/test_railway.c backend/railway.c backend/railway.h
	$(CC) $(CFLAGS) -g -fsanitize=address,undefined backend/test_railway.c backend/railway.c -o $@

test: test_railway
	./test_railway

clean:
	rm -rf railway test_railway test_railway.dSYM

.PHONY: all run test clean
