all: Julia

Julia: Julia.o
	mpicc -g -o bin/Julia obj/Julia.o

Julia.o:
	mpicc -g -c -o obj/Julia.o src/conjuntoJuliaP.c

clean:
	rm obj/*.o 
	rm bin/* 
	rm *.raw