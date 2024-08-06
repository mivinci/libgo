include ../config.mk

headers := $(wildcard *.h)

objects := \
	asm_$(ARCH).o \
	timer.o \
	sched.o

ifeq ($(SYS), Darwin)
	objects += poller_kqueue.o
endif

ifeq ($(SYS), Linux)
	objects += poller_epoll.o
endif

CFLAGS += -I../include

target := libcore.a
ifdef TEST
	target  = test.out
	tests   := $(wildcard *_test.c)
	objects += $(tests:.c=.o)
	CFLAGS  += -I../libs/utest.h
endif


all: $(target)


libcore.a: $(objects)
	$(AR) rcs $@ $^


test.out: $(headers) libcore.a
	$(CC) $(CFLAGS) -o $@ libcore.a


.PHONY: clean
clean:
	rm *.a
	rm *.o
