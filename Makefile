.POSIX:
.SUFFIXES:
.SUFFIXES: .o .c
.PHONY: all clean install

PREFIX = /usr/local
CFLAGS = -g -DDEBUG

OBJS := elfload.o elfreloc_i386.o elfreloc_amd64.o elfreloc_aarch64.o
HDRS := elfload.h elfarch.h elf.h
ALL  := libelfload.a elfloader

all: $(ALL)

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS)

libelfload.a: $(OBJS)
	ar cru $@ $(OBJS)

elfloader: elfloader.c libelfload.a
	$(CC) -g -o elfloader elfloader.c libelfload.a $(CFLAGS)

clean:
	rm $(ALL) $(OBJS)

install:
	install -m644 libelfload.a $(DESTDIR)$(PREFIX)/lib
	install -m644 $(HDRS) $(DESTDIR)$(PREFIX)/include
