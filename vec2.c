/* 
 * vec.c: Example of SSE usage. 
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xmmintrin.h>

#include "hpctimer.h"

enum { 
    N = 1024 * 1024 * 4,
    NREPS = 10
};

void fun_def(float *a, float *b, int n)
{
    int i;
		
    for (i = 0; i < n; i++) {
        b[i] = sqrt(a[i] * a[i] + 2.8f);
    }
}

void fun_sse(float *a, float *b, int n)
{
    /* TODO */
    int i, k;
    __m128 x, z;
    __m128 *aa = (__m128 *)a;
    __m128 *bb = (__m128 *)b;
	
    k = n / 4;
    z = _mm_set_ps1(2.8f);
	                        
    for (i = 0; i < k; i++) {
        x = _mm_mul_ps(*aa, *aa);
        x = _mm_add_ps(x, z);      
        *bb = _mm_sqrt_ps(x);
        aa++;
        bb++;		
    }
}

void print_vec(float *a, int n)
{
    int i;
    
    for (i = 0; i < n; i++) {
        printf("%14.6f ", a[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
	int i;
	float *a, *b;
    double t;
    		
    a = (float *)_mm_malloc(sizeof(float) * N, 16);
    b = (float *)_mm_malloc(sizeof(float) * N, 16);
	for (i = 0; i < N; i++) {
		a[i] = 1.0;
	}
			      
    t = hpctimer_getwtime();
    for (i = 0; i < NREPS; i++) {
        fun_def(a, b, N);
	//fun_sse(a, b, N);
    }
    t = hpctimer_getwtime() - t;
    t = t / NREPS;
       
    //print_vec(b, N);
    
    printf("Elapsed time: %.6f sec.\n", t);

	_mm_free(a);
	_mm_free(b);

    return 0;
}
