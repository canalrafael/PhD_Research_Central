override CFLAGS += -O3 -pthread -Wno-attributes

# Only build the static library (.a)
all: libkdump.a

libkdump.o: libkdump.c
	$(CC) -c -fPIC libkdump.c -o libkdump.o $(CFLAGS)
    
libkdump.a: libkdump.o
	ar rcs libkdump.a libkdump.o 

clean:
	rm -f *.o *.so *.a