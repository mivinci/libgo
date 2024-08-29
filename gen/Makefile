CC     := cc
CFLAGS := -O2

all: gen

gen: offsets.h

offsets.h: offsets.c
	$(CC) $(CFLAGS) -o offsets.exe $<
	./offsets.exe
