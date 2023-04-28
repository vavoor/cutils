CFLAGS=-O3  -g -I include
CC=clang
AR= ar

all: lib/cutils.a

tst: lib/cutils.a
	$(CC) $(CFLAGS) -g -o test/t_alist.bin test/t_alist.c lib/cutils.a && test/t_alist.bin
	$(CC) $(CFLAGS) -g -o test/t_hmap.bin test/t_hmap.c lib/cutils.a && test/t_hmap.bin
	$(CC) $(CFLAGS) -g -o test/t_err.bin test/t_err.c lib/cutils.a && test/t_err.bin
	$(CC) $(CFLAGS) -g -o test/t_futils.bin test/t_futils.c lib/cutils.a && test/t_futils.bin

clean:;
	rm -rf obj/* lib/*

lib/cutils.a: obj/alist.o obj/hmap.o obj/err.o obj/futils.o
	$(AR) -rsc lib/cutils.a obj/alist.o obj/hmap.o obj/err.o obj/futils.o

obj/alist.o: src/alist.c include/alist.h
	$(CC) $(CFLAGS) -c -o obj/alist.o src/alist.c

obj/hmap.o: src/hmap.c include/hmap.h include/alist.h
	$(CC) $(CFLAGS) -c -o obj/hmap.o src/hmap.c

obj/err.o: src/err.c include/err.h
	$(CC) $(CFLAGS) -c -o obj/err.o src/err.c

obj/futils.o: src/futils.c include/futils.h
	$(CC) $(CFLAGS) -c -o obj/futils.o src/futils.c
