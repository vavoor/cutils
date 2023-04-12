CFLAGS= -g -I include
CC= gcc
AR= ar

all: lib/cutils.a

tst: lib/cutils.a
	$(CC) $(CFLAGS) -g -o test/t_alist.bin test/t_alist.c lib/cutils.a && test/t_alist.bin

clean:;
	rm -rf obj/* lib/*

lib/cutils.a: obj/alist.o
	$(AR) -rsc lib/cutils.a obj/alist.o

obj/alist.o: src/alist.c include/alist.h
	$(CC) $(CFLAGS) -c -o obj/alist.o src/alist.c
