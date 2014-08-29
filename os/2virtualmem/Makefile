virtualmem: lib.o policy.o virtualmem.o
	gcc -o virtualmem lib.o policy.o virtualmem.o
lib.o: lib.c
	gcc -c lib.c
policy.o: policy.c
	gcc -c policy.c
virtualmem.o: virtualmem.c
	gcc -c virtualmem.c
depend:
	makedepend virtualmem.c lib.c policy.c
clean:
	rm *.o
