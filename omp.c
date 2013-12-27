/* Исходный код OpenMP-программы */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>

/* Начальные условия */
#define X_MIN   0
#define X_MAX   1
#define Y_MIN   0
#define Y_MAX   0.5
#define TIME 10

int size;/*Кол-во потоков*/
int Nx, Ny;
double hx, hy, ht;

double **Y;
double **Y_next;

/* Лямда */
double L(int i, int j)
{
	return (0.25 <= hx * j && hx * j <= 0.65 &&
			 0.1 <= hy * i && hy * i <= 0.25) ? 0.001 : 0.00001;
}

double Ay(int i, int j){return -(L(i, j - 1) + L(i, j)) / (4 * hx * hx);}

double By(int i, int j){return -(L(i, j + 1) + L(i, j)) / (4 * hx * hx);}

double Cy(int i, int j){return 1 / ht - Ay(i, j) - By(i, j);}

double Ax(int i, int j){return -(L(i - 1, j) + L(i, j)) / (4 * hy * hy);}

double Bx(int i, int j){return -(L(i + 1, j) + L(i, j)) / (4 * hy * hy);}

double Cx(int i, int j){return 1 / ht - Ax(i, j) - Bx(i, j);}

void init_y()
{
	int i, j;
	Y = malloc(Ny * sizeof(double*));
	Y_next = malloc(Ny * sizeof(double*));
	for (i = 0; i < Ny; i++) {
		Y[i] = malloc(Nx * sizeof(double));
		Y_next[i] = malloc(Nx * sizeof(double));
		for (j = 0; j < Nx; j++)
			Y[i][j] = Y_next[i][j] = 300;
	}

	for (i = 0; i < Ny; i++) {
		Y[i][0] = Y_next[i][0] = 600;
		Y[i][Nx - 1] = Y_next[i][Nx - 1] = 1200;
	}

	for (j = 1; j < Nx - 1; j++) {
		Y[0][j] = Y_next[0][j] = 600 * (1 + hx * j);
		Y[Ny - 1][j] = Y_next[Ny - 1][j] = 600 * (1 + pow(hx * j, 3.0));
	}
}

int main(int argc, char *argv[])
{
	int i, j, t;
	double t_calc = 0;

	if (argc < 3) {
		printf("Usage:\n\t%s Nx Ny\n", argv[0]);
		return 1;
	}

	Nx = atoi(argv[1]);
	Ny = atoi(argv[2]);
	hx = (X_MAX - X_MIN) / (Nx - 1.0);
	hy = (Y_MAX - Y_MIN) / (Ny - 1.0);
	ht = (hx > hy ? hy : hx);
	t = ceil(TIME / ht);
	
	/* Установка начальных значений температуры */
	init_y();

	t_calc -= omp_get_wtime();
	while (t--) {
        #pragma omp parallel shared(Y, Y_next)
		{           
            #pragma omp for private(j, i)
			for (j = 1; j < Nx - 1; j++) {
				double alpha[Ny], beta[Ny];
				alpha[0] = 0;
				beta[0] = Y[0][j];

				for (i = 1; i < Ny - 1; i++) {
					double F = Y[i][j] / ht + ((L(i + 1, j) + L(i, j)) *
							   (Y[i + 1][j] - Y[i][j]) - (L(i - 1, j) + L(i, j))
							   * (Y[i][j] - Y[i - 1][j])) / (4 * hy * hy);
					alpha[i] = - By(i, j) / (Cy(i, j) + Ay(i, j) * alpha[i - 1]);
					beta[i] = (F - Ay(i, j) * beta[i - 1]) / (Cy(i, j) + Ay(i, j)
							  * alpha[i - 1]);
				}

				for (i = Ny - 2; i >= 1; i--)
					Y_next[i][j] = alpha[i] * Y_next[i + 1][j] + beta[i];
			}

            #pragma omp for private(i, j)
			for (i = 1; i < Ny - 1; i++) {
				double alpha[Nx], beta[Nx];
				alpha[0] = 0;
				beta[0] = Y_next[i][0];

				for (j = 1; j < Nx - 1; j++) {
					double F = Y_next[i][j] / ht + ((L(i, j + 1) + L(i, j)) * 
							  (Y_next[i][j + 1] - Y_next[i][j]) - (L(i, j - 1) + 
							  L(i, j)) * (Y_next[i][j] - Y_next[i][j - 1])) / 
							  (4 * hx * hx);
					alpha[j] = - Bx(i, j) / (Cx(i, j) + Ax(i, j) * alpha[j - 1]);
					beta[j] = (F - Ax(i, j) * beta[j - 1]) / (Cx(i, j) + Ax(i, j)
							  * alpha[j - 1]);
				}

				for (j = Nx - 2; j > 0; j--)
					Y[i][j] = alpha[j] * Y[i][j + 1] + beta[j];
			}
		}
	}
	t_calc += omp_get_wtime();
	
	#pragma omp parallel
	#pragma omp master
	size = omp_get_num_threads();	
	
	/* Вывод результатов */
	printf("%d\t%f\n", size, t_calc);
	printf("Temperature\n");
	printf("%.15f\n", Y[Ny / 2][Nx / 2]);

/*	for(i = 0; i < Ny; ++i){
		for(j = 0; j < Nx; ++j)
			printf("%11.3f", Y[i][j]);
			printf("\n");
	}
*/
	
	/*Освобождение памяти*/
	for(i = 0; i < Ny; ++i){	
		free(Y[i]);
		free(Y_next[i]);
	}
	free(Y);
	free(Y_next);
	
	return 0;
}
