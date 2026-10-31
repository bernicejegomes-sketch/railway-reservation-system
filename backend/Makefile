# Bernice's C Core Makefile (Component-level build and test runner)
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Iinclude

SRCS = src/validation.c \
       src/confirmed_list.c \
       src/waiting_queue.c \
       src/seat_manager.c \
       src/train.c \
       src/booking_history.c \
       src/booking.c \
       src/cancellation.c \
       src/railway.c

TEST_TARGETS = test_waiting_queue \
               test_confirmed_list \
               test_seat_manager \
               test_booking \
               test_cancellation \
               test_all

.PHONY: all test clean

all: test

test_waiting_queue: tests/test_waiting_queue.c src/waiting_queue.c
	$(CC) $(CFLAGS) $^ -o $@

test_confirmed_list: tests/test_confirmed_list.c src/confirmed_list.c
	$(CC) $(CFLAGS) $^ -o $@

test_seat_manager: tests/test_seat_manager.c src/seat_manager.c src/train.c src/confirmed_list.c src/waiting_queue.c src/validation.c
	$(CC) $(CFLAGS) $^ -o $@

test_booking: tests/test_booking.c $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

test_cancellation: tests/test_cancellation.c $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

test_all: tests/test_all.c $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

test: $(TEST_TARGETS)
	@echo Running component unit tests...
	./test_waiting_queue
	./test_confirmed_list
	./test_seat_manager
	./test_booking
	./test_cancellation
	./test_all
	@echo All C core unit tests completed successfully.

clean:
ifeq ($(OS),Windows_NT)
	-del /f /q test_waiting_queue.exe test_confirmed_list.exe test_seat_manager.exe test_booking.exe test_cancellation.exe test_all.exe *.o 2>NUL
else
	rm -f $(TEST_TARGETS) *.o *.exe
endif
