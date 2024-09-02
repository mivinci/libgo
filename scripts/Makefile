CC     := cc
CFLAGS := -O2
DIR    := ../gen

all: pre offsets.h

.PHONY: pre
pre:
	mkdir -p $(DIR)

offsets.h: offsets.c
	$(CC) $(CFLAGS) -o offsets.out $<
	./offsets.out $(DIR)/offsets.h
