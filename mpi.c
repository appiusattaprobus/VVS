/* Исходный код MPI-программы */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<mpi.h>

/* Начальные условия */
#define X_MIN   0
#define X_MAX   1
#define Y_MIN   0
#define Y_MAX   0.5
#define TIME 10

int rank, size, part_y;
int Nx, Ny;
double hx, hy, ht;

double **Y;
double **Y_next;

void mov(int a[], int b[])
{
    int j;
    for(j = 0; j < Nx; ++j)
		a[j] = b[j];
}

/* Лямда */
double L(int i, int j)
{
    return (0.25 <= hx * j && hx * j <= 0.65 &&
			 0.1 <= hy * i && hy * i <= 0.25) ? 0.01 : 0.0001;
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
	Y = malloc((part_y + 2) * sizeof(double*));
	Y_next = malloc((part_y + 2) * sizeof(double*));
	for (i = 0; i < part_y + 2; i++) {
		Y[i] = malloc(Nx * sizeof(double));
		Y_next[i] = malloc(Nx * sizeof(double));
		for (j = 0; j < Nx; j++)
			Y[i][j] =  Y_next[i][j] = 300;
	}

	for (i = 1; i <= part_y; i++) {
		Y[i][0] = Y_next[i][0] = 600;
		Y[i][Nx - 1] = Y_next[i][Nx - 1] = 1200;
	}

	if (rank == 0)
		for (j = 1; j < Nx - 1; j++)
			Y[1][j] = Y_next[1][j] = 600 * (1 + hx * j);

	if (rank == size - 1)
		for (j = 1; j < Nx - 1; j++)
			Y[part_y][j] = Y_next[part_y][j] = 600 * (1 + pow(hx * j, 3.0));
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;

	if (argc < 3) {
		printf("Usage:\n\t%s Nx Ny\n", argv[0]);
		return 1;
	}

	Nx = atoi(argv[1]);
	Ny = atoi(argv[2]);
	hx = (X_MAX - X_MIN) / (Nx - 1.0);
	hy = (Y_MAX - Y_MIN) / (Ny - 1.0);
	ht = (hx > hy ? hy : hx);

	double t_calc = 0, t_mpi = 0;

	part_y = Ny / size + (rank == size - 1 ? Ny % size : 0);

	/* Установка начальных значений температуры */
	init_y();

	int i, j, t = ceil(TIME / ht);
	int first_y = 1 + (rank == 0);
	int last_y = part_y - (rank == size - 1);

	while (t--) {
		t_mpi -= MPI_Wtime();
		//Четные процессы(кроме последнего) передают свою последнюю строку следующему процессу
		if((rank % 2 == 0) && (rank != size - 1)){
			MPI_Send(&Y[part_y][0], Nx, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&Y[part_y + 1][0], Nx, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &status);
		}
		//Нечетные процессы принимаю последнюю строку предыдущего процесса
		else if(rank % 2 == 1){
			MPI_Recv(&Y[0][0], Nx, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
			MPI_Send(&Y[1][0], Nx, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
		}

        //Четные процессы(кроме первого) принимают последнюю строку предыдущего процесса
		if((rank % 2 == 0) && (rank != 0)){
            MPI_Recv(&Y[0][0], Nx, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
		    MPI_Send(&Y[1][0], Nx, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
		}
        //Нечетные процессы (кроме последнего) отправляют последнюю строку следующему процессу
		else if((rank % 2 == 1) && (rank != size - 1)){
		    MPI_Send(&Y[part_y][0], Nx, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
		    MPI_Recv(&Y[part_y + 1][0], Nx, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &status);
		}
		t_mpi += MPI_Wtime();
		t_calc -= MPI_Wtime();
		//Четные процессы(кроме последнего) передают свою последнюю строку следующему процессу
		if((rank % 2 == 0) && (rank != size - 1)){
			MPI_Send(&Y_next[part_y][0], Nx, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&Y_next[part_y + 1][0], Nx, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &status);
		}
		//Нечетные процессы принимаю последнюю строку предыдущего процесса
		else if(rank % 2 == 1){
			MPI_Recv(&Y_next[0][0], Nx, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
			MPI_Send(&Y_next[1][0], Nx, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
		}

        //Четные процессы(кроме первого) принимают последнюю строку предыдущего процесса
		if((rank % 2 == 0) && (rank != 0)){
            MPI_Recv(&Y_next[0][0], Nx, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
		    MPI_Send(&Y_next[1][0], Nx, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
		}
        //Нечетные процессы (кроме последнего) отправляют последнюю строку следующему процессу
		else if((rank % 2 == 1) && (rank != size - 1)){
		    MPI_Send(&Y_next[part_y][0], Nx, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
		    MPI_Recv(&Y_next[part_y + 1][0], Nx, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &status);
		}

		for (j = 1; j < Nx - 1; j++){
			double alpha[part_y + 1], beta[part_y + 1];
			int shift = rank * (part_y - Ny % size) - 1;

			alpha[first_y - 1] = 0;
			beta[first_y - 1] = Y[first_y - 1][j];

			for (i = first_y; i <= last_y; i++) {
				double F = Y[i][j] / ht + ((L(i + 1 + shift, j) + L(i + shift, j)) *
						   (Y[i + 1][j] - Y[i][j]) - (L(i - 1 + shift, j) + L(i + shift, j))
						   * (Y[i][j] - Y[i - 1][j])) / (4 * hy * hy);
				alpha[i] = - By(i + shift, j) / (Cy(i + shift, j) + Ay(i + shift, j) * alpha[i - 1]);
				beta[i] = (F - Ay(i + shift, j) * beta[i - 1]) / (Cy(i + shift, j) + Ay(i + shift, j)
						  * alpha[i - 1]);
			}

			for (i = last_y; i >= first_y; i--)
				Y_next[i][j] = alpha[i] * Y_next[i + 1][j] + beta[i];
		}

		for (i = first_y; i <=last_y; i++) {
			double alpha[Nx], beta[Nx];
			int shift = rank * (part_y - Ny % size) - 1;

			alpha[0] = 0;
			beta[0] = Y_next[i][0];

			for (j = 1; j < Nx - 1; j++) {
			double F = Y_next[i][j] / ht + ((L(i + shift, j + 1) + L(i + shift, j)) * 
						  (Y_next[i][j + 1] - Y_next[i][j]) - (L(i + shift, j - 1) + 
						  L(i + shift, j)) * (Y_next[i][j] - Y_next[i][j - 1])) / 
						  (4 * hx * hx);
				alpha[j] = - Bx(i + shift, j) / (Cx(i + shift, j) + Ax(i + shift, j) * alpha[j - 1]);
				beta[j] = (F - Ax(i + shift, j) * beta[j - 1]) / (Cx(i + shift, j) + Ax(i + shift, j)
						  * alpha[j - 1]);
			}

			for (j = Nx - 2; j > 0; j--)
				Y[i][j] = alpha[j] * Y[i][j + 1] + beta[j];
		}
		t_calc += MPI_Wtime();
	}
	
	double res[Ny][Nx], buf[part_y][Nx];
	
	for(i = 1; i <= part_y; ++i)
		for(j = 0; j < Nx; ++j)
			buf[i - 1][j] = Y[i][j];
	t_mpi -= MPI_Wtime();
	MPI_Gather(buf, Nx * (part_y - (rank == size - 1 ? Ny % size : 0)), MPI_DOUBLE, res, Nx * (part_y - (rank == size - 1 ? Ny % size : 0)), MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if(Ny % size != 0){
		int k = Ny % size;
		if(rank == 0)MPI_Recv(&res[Ny - k][0], Nx * k, MPI_DOUBLE, size - 1, 0, MPI_COMM_WORLD, &status);
		if(rank == size - 1)MPI_Send(&buf[last_y - k + 1][0], Nx * k, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	t_mpi += MPI_Wtime();

	// Output: size, t_calc, t_mpi 
	if (rank == 0)
	{
		printf("%d\t%f\t%f\n", size, t_calc, t_mpi);
		printf("%.15f\n", res[Ny / 2][Nx / 2]);
	}

/*	if(rank == 0){
		for(i = 0; i < Ny; ++i){
			for(j = 0; j < Nx; ++j)
				printf("%11.3f", res[i][j]);
			printf("\n");
		}
	}*/
	for(i = 0; i < part_y + 2; ++i){
		free(Y[i]);
		free(Y_next[i]);
	}
	free(Y);
	free(Y_next);
	MPI_Finalize();

	return 0;
}
