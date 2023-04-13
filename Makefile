CFLAGS= -g -I include
CC= gcc
AR= ar

all: lib/cutils.a

tst: lib/cutils.a
	$(CC) $(CFLAGS) -g -o test/t_alist.bin test/t_alist.c lib/cutils.a && test/t_alist.bin
	$(CC) $(CFLAGS) -g -o test/t_hmap.bin test/t_hmap.c lib/cutils.a && test/t_hmap.bin

clean:;
	rm -rf obj/* lib/*

lib/cutils.a: obj/alist.o obj/hmap.o
	$(AR) -rsc lib/cutils.a obj/alist.o obj/hmap.o

obj/alist.o: src/alist.c include/alist.h
	$(CC) $(CFLAGS) -c -o obj/alist.o src/alist.c

obj/hmap.o: src/hmap.c include/hmap.h include/alist.h
	$(CC) $(CFLAGS) -c -o obj/hmap.o src/hmap.c
