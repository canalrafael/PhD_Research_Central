override CFLAGS += -O2 -pthread -Wno-attributes -march=armv8-a -static -fPIC -D_GNU_SOURCE
CC=aarch64-linux-gnu-gcc

SOURCES := $(wildcard *.c)
BINARIES := $(SOURCES:%.c=%)

all: $(BINARIES)

libkdump/libkdump.a: libkdump/libkdump.c
	$(MAKE) -C libkdump CC=$(CC) CFLAGS="$(CFLAGS)"

%: %.c libkdump/libkdump.a
	$(CC) $< -o $@ -Llibkdump -Ilibkdump -lkdump $(CFLAGS)
    
clean:
	rm -f *.o $(BINARIES)
	$(MAKE) clean -C libkdump