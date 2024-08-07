CFLAGS=-O2 -g -I include
#CFLAGS=-g -I include
#CC=clang
CC=gcc
AR= ar

OBJ=obj/alist.o obj/hmap.o obj/err.o obj/futils.o obj/llist.o obj/template.o obj/str.o obj/json.o obj/pmap.o obj/plist.o obj/pset.o obj/sym.o

all: lib/cutils.a

tst: lib/cutils.a
	$(CC) $(CFLAGS) -g -o test/t_alist.bin test/t_alist.c lib/cutils.a && test/t_alist.bin
	$(CC) $(CFLAGS) -g -o test/t_hmap.bin test/t_hmap.c lib/cutils.a && test/t_hmap.bin
	$(CC) $(CFLAGS) -g -o test/t_err.bin test/t_err.c lib/cutils.a && test/t_err.bin
	$(CC) $(CFLAGS) -g -o test/t_futils.bin test/t_futils.c lib/cutils.a && test/t_futils.bin
	$(CC) $(CFLAGS) -g -o test/t_llist.bin test/t_llist.c lib/cutils.a && test/t_llist.bin
	$(CC) $(CFLAGS) -g -o test/t_template.bin test/t_template.c lib/cutils.a && test/t_template.bin
	$(CC) $(CFLAGS) -g -o test/t_json.bin test/t_json.c lib/cutils.a && test/t_json.bin
	$(CC) $(CFLAGS) -g -o test/t_pset.bin test/t_pset.c lib/cutils.a && test/t_pset.bin

clean:;
	rm -rf obj/* lib/*

lib/cutils.a: $(OBJ)
	$(AR) -rsc lib/cutils.a $(OBJ)

obj/alist.o: src/alist.c include/alist.h
	$(CC) $(CFLAGS) -c -o obj/alist.o src/alist.c

obj/hmap.o: src/hmap.c include/hmap.h include/alist.h
	$(CC) $(CFLAGS) -c -o obj/hmap.o src/hmap.c

obj/err.o: src/err.c include/err.h
	$(CC) $(CFLAGS) -c -o obj/err.o src/err.c

obj/futils.o: src/futils.c include/futils.h
	$(CC) $(CFLAGS) -c -o obj/futils.o src/futils.c

obj/llist.o: src/llist.c include/llist.h
	$(CC) $(CFLAGS) -c -o obj/llist.o src/llist.c

obj/template.o: src/template.c include/template.h
	$(CC) $(CFLAGS) -c -o obj/template.o src/template.c

obj/str.o: src/str.c include/str.h
	$(CC) $(CFLAGS) -c -o obj/str.o src/str.c

obj/json.o: src/json.c include/json.h
	$(CC) $(CFLAGS) -c -o obj/json.o src/json.c

obj/pmap.o: src/pmap.c include/pmap.h
	$(CC) $(CFLAGS) -c -o obj/pmap.o src/pmap.c

obj/plist.o: src/plist.c include/plist.h
	$(CC) $(CFLAGS) -c -o obj/plist.o src/plist.c
    
obj/pset.o: src/pset.c include/pset.h
	$(CC) $(CFLAGS) -c -o obj/pset.o src/pset.c
    
obj/sym.o: src/sym.c include/sym.h include/pmap.h include/plist.h
	$(CC) $(CFLAGS) -c -o obj/sym.o src/sym.c
