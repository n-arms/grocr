# lc - load cell driver
# Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
# see LICENCE file for licensing information

.POSIX:

include config.mk

SRC = lc.c
HED = lc.h
OBJ = $(SRC:.c=.o)

all: $(OBJ)

$(OBJ): $(HED) config.mk

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ)

.PHONY: all clean

