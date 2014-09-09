citizen: lib.o citizen.o
	gcc -o citizen citizen.o lib.o
lib.o: lib.c
	gcc -c lib.c
citizen.o: citizen.c
	gcc -c citizen.c
depend:
	citizen.c lib.c
clean:
	mv *.o
