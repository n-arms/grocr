# ham - hamming error correction codes
# Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
# see LICENCE file for licensing information

WPROFILE = -Wall -Wextra -Wstrict-prototypes -Wmissing-declarations -Wshadow \
-Wswitch-default -Wunreachable-code -Wcast-align -Wpointer-arith -Wcast-qual \
-Wbad-function-cast -Winline -Wundef -Wnested-externs -Wwrite-strings \
-Wno-unused-parameter -Wfloat-equal -Wpedantic
STD = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
LIB =

CFLAGS = $(WPROFILE) $(STD) -Os
LDFLAGS = $(LIB)

