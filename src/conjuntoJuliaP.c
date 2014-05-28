/*
 ============================================================================
 Name        : conjuntoJulia.c
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
#define Col       512                    /* Numero de columnas de la imagen.*/
#define Fil       512                    /* Numero de filas de la imagen.*/
#define Ventx     -2.0                   /* Esquina inferior izquierda del espacio.*/
#define Venty     -2.0                   /* Esquina superior derecha del espacio.*/
#define Tamx      4                      /* Ancho de la ventana.*/
#define Tamy      4                     /* Alto de la ventana.*/
#define Incx      Tamx/Col
#define Incy      Tamy/Fil

/*****************************************
 Para el conjunto de julia se resuelve:
 f_c(z) = z^2 + c
 Siendo c un valor constante complejo,
 z una variable compleja donde
 z0 = coordenadas del punto
 ******************************************/

int main(int argc, char* argv[]) {

	int size, rank;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	float zr, zi; /*Parte real e imaginaria de z.*/
	float cr, ci; /*Parte real e imaginaria de c.*/
	float zrs, zis;
	int i, j;

	char im[Fil][Col]; /*Reserva de memoria est������tica para la imagen*/
	char im_local[Fil/size][Col/size]; // Reserva de memoria para la porción de imágen.
	char color;
	FILE *fi;
	clock_t t0, t1;
	double tiempo;
	int indice;

	int root;

	//char salida[12];
	//snprintf(salida, 12, "Salida%d.raw",rank);
	//fi = fopen(salida, "wb");

	fi = fopen("Salida.raw", "wb");
	if (!fi) {
		printf("No es posible abrir el fichero de salida\n");
		exit(1);
	}

	//Asignamos el valor de c para el conjunto de julia en particular, parte real e imaginaria.
	cr = -0.8;
	ci = 0.156;
	t0 = clock();


		for (j = rank*(Fil/size); j < (rank+1)*(Fil/size); j++)

			for (i = 0; i < Col; i++) {
				zi = (float) Venty + (float) j * Incy; // z0=coordenadas del punto (parte imaginaria).
				zr = (float) Ventx + (float) i * Incx; // z0=coordenadas del punto (parte real).
				zrs = zis = (float) 0; // Se inicializan los cuadrados de z, su parte real al cuadrado, zrs y su parte imaginaria al cuadrado (zis).
				color = 0; // Cada punto se colorea seg������n el n������mero de iteraciones necesarias para escapar.
				indice = 0; //Se contabiliza el n������mero de iteraciones utilizando un valor entero y no char.
				while (zrs + zis < (float) 4 && (int) color < 256) {
					//Calculo z^2 + c como zr^2-zi^2+2*zr*zi+c.
					zrs = zr * zr; // Calculo zr^2.
					zis = zi * zi; // Calculo zi^2.
					zi = 2 * zr * zi + ci; // Calculo la parte imaginaria de z --> 2*zr*zi+ci.
					zr = zrs - zis + cr; // Calculo la parte real de z --> x^2-y^2+cr.
					indice++;
					color++; // Cada punto se colorea seg������n el n������mero de iteraciones necesarias para escapar.
							 // Los puntos que no consiguen escapar se presentan en negro.
				}
				im_local[j][i] = color - 1; // Asigno el color.



				//fprintf(stderr, "\n(%dx%d)\n", j, i);
			}

	MPI_Gather(im_local, 1, MPI_CHAR, im, 1, MPI_CHAR, 0, MPI_COMM_WORLD);


	t1 = clock();
	// Paso los valores de la matriz al fichero Salida.
	if(rank == 0){
		for (j = 0; j < Fil; j++)
			fwrite(im[j], sizeof(char), Col, fi);
		fclose(fi);


		tiempo = (double) (t1 - t0);
		fprintf(stderr, "\nTiempo Conjunto de Julia(%dx%d)=%.16g milisegundos\n", Col, Fil, tiempo);
	}

	MPI_Finalize();
	return EXIT_SUCCESS;
}


