/*
 * prog.c:
 *
 */

#include <immintrin.h> /* SSE3 */
#include <stdio.h>

#include "hpctimer.h"

enum { N = 1024 * 1024, NREPS = 10 };

float reduction_sum(float *v, int n)
{
    int i;
    float sum = 0.0;
    
    for (i = 0; i < n; i++) {
        sum += v[i];
    }
    return sum;
}

float reduction_sum_sse(float *v, int n)
{
	int i;
	
	float sum;
	
	__m128 *v4 = (__m128 *)v;
	__m128 vsum = _mm_set1_ps(0.0f);
	
	for (i = 0; i < n / 4; i++)
		vsum = _mm_add_ps(vsum, v4[i]);
	
	/* Horizontal sum: | a3+a2 | a1+a0 | a3+a2 | a1+a0 | */
	vsum = _mm_hadd_ps(vsum, vsum);
	
	/* Horizontal sum: | a3+a2+a1+a0 | -- | -- | -- | */
	vsum = _mm_hadd_ps(vsum, vsum);
	
	_mm_store_ss(&sum, vsum);
	
	return sum;

}

int main()
{
	int i;
	float *a;
    double t, sum;
    		
    a = (float *)_mm_malloc(sizeof(*a) * N, 16);
	for (i = 0; i < N; i++)
		a[i] = 1.0;
		
	printf("N=%d\n", N);

    t = hpctimer_getwtime();
    for (i = 0; i < NREPS; i++) {
        sum = reduction_sum(a, N);
    }
    t = (hpctimer_getwtime() - t) / NREPS;
    printf("Reduction sum: %.4f (real %.4f)\n", sum, (float)N);
    printf("Elapsed time: %.6f sec.\n", t);
    
   
    t = hpctimer_getwtime();
    for (i = 0; i < NREPS; i++) {
        sum = reduction_sum_sse(a, N);
    }
    t = (hpctimer_getwtime() - t) / NREPS;
    printf("Reduction sum: %.4f (real %.4f)\n", sum, (float)N);
    printf("Elapsed time: %.6f sec.\n", t);

	_mm_free(a);
    return 0;
}
