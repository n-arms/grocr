# ham - hamming error correction codes
# Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
# see LICENCE file for licensing information

.POSIX:

include config.mk

SRC = ham.c ham-test.c
HED = ham.h
OBJ = $(SRC:.c=.o)

all: $(OBJ) ham-test

$(OBJ): $(HED) config.mk

.c.o:
	$(CC) $(CFLAGS) -c $<

ham-test: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) ham-test

.PHONY: all clean

