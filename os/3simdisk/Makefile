simdisk: lib.o policy.o simdisk.o
	gcc -o simdisk lib.o policy.o simdisk.o
lib.o: lib.c
	gcc -c lib.c
policy.o: policy.c
	gcc -c policy.c
simdisk.o: simdisk.c
	gcc -c simdisk.c
depend:
	makedepend simdisk.c lib.c policy.c
clean:
	rm *.o
