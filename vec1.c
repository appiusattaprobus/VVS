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
    N = 1024 * 1024 * 4, //(10), *4(22)
    NREPS = 10
};

void fun_def(float *a, float *b, float *c, int n)
{
    int i;
		
    for (i = 0; i < n; i++) {
        c[i] = sqrt(a[i] * a[i] + b[i] * b[i]) + 0.5f;
    }
}

void fun_sse(float *a, float *b, float *c, int n)
{
    int i, k;
    __m128 x, y, z;
    __m128 *aa = (__m128 *)a;
    __m128 *bb = (__m128 *)b;
    __m128 *cc = (__m128 *)c;
	
    k = n / 4;
    z = _mm_set_ps1(0.5f);
	                        
    for (i = 0; i < k; i++) {
        x = _mm_mul_ps(*aa, *aa);  
        y = _mm_mul_ps(*bb, *bb);
        x = _mm_add_ps(x, y);    
        x = _mm_sqrt_ps(x);      
        *cc = _mm_add_ps(x, z);
        aa++;
        bb++;
        cc++;		
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
	float *a, *b, *c;
    double t;
    		
    a = (float *)_mm_malloc(sizeof(float) * N, 16);
    b = (float *)_mm_malloc(sizeof(float) * N, 16);
    c = (float *)_mm_malloc(sizeof(float) * N, 16);
	for (i = 0; i < N; i++) {
		a[i] = 1.0;
		b[i] = 2.0;
	}
			      
    t = hpctimer_getwtime();
    for (i = 0; i < NREPS; i++) {
        fun_sse(a, b, c, N);
        //fun_def(a, b, c, N);
    }
    t = hpctimer_getwtime() - t;
    t = t / NREPS;
       
    //print_vec(c, N);
    
    printf("Elapsed time: %.6f sec.\n", t);

	_mm_free(a);
	_mm_free(b);
	_mm_free(c);

    return 0;
}
