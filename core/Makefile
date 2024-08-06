include ../config.mk

headers := $(wildcard *.h)

objects := \
	asm_$(ARCH).o \
	timer.o \
	sched.o

ifeq ($(SYS), Darwin)
	objects += \
		poller_kqueue.o \
		sys_darwin.o
endif

ifeq ($(SYS), Linux)
	objects += \
		poller_epoll.o \
		sys_linux.o
endif

CFLAGS += -I../include

target := libcore.a
ifdef TEST
	target  = test.out
	tests   := $(wildcard *test.c)
	objects += $(tests:.c=.o)
endif


all: $(target)


libcore.a: $(objects)
	$(AR) rcs $@ $^


test.out: $(headers) libcore.a
	$(CC) $(CFLAGS) -o $@ -L. -lcore


.PHONY: clean
clean:
	rm *.a
	rm *.o
