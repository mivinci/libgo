ARCH   := $(shell uname -m)
SYS    := $(shell uname -s)
CC     := cc
AR     := ar
CFLAGS := -Wall -std=c99
PWD    := $(shell pwd)

ifdef DEBUG
	CFLAGS += -g -DDEBUG
endif

ifdef TEST
	CFLAGS += -DTEST
endif

ifdef VERSION
	CFLAGS += -DVERSION=$(VERSION)
endif

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<
