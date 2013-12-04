/*
 * prog.c:
 *
 */
#include <xmmintrin.h>  /* SSE3 */
#include <stdio.h>
#include <float.h>      /* for FLT_MAX */

#include "hpctimer.h"

enum { N = 16 * 1024 * 1024, NREPS = 10 };

float vmin(float *v, int n)
{
    int i;
    float minval = FLT_MAX;
    
    for (i = 0; i < n; i++) {
        if (v[i] < minval)
            minval = v[i];
    }
    return minval;
}

float vmin_sse(float *v, int n)
{
    /* TODO */
	int i;
	float res;
	__m128 *f4 = (__m128 *)v;
	__m128 minval = _mm_setzero_ps();

	for(i=0;i<n/4;i++)
		minval = _mm_min_ps(minval, f4[i]);
	/* Horisontal min */
	minval = _mm_min_ps(minval, _mm_shuffle_ps(minval, minval, 0x93));
	minval = _mm_min_ps(minval, _mm_shuffle_ps(minval, minval, 0x93));
	minval = _mm_min_ps(minval, _mm_shuffle_ps(minval, minval, 0x93));
	_mm_store_ss(&res, minval);
    return res;
}

int main()
{
	int i;
	float *a, val;
    double t;
    		
    a = (float *)_mm_malloc(sizeof(*a) * N, 16);
	for (i = 0; i < N; i++)
		a[i] = (float)(N - i);

    t = hpctimer_getwtime();
    for (i = 0; i < NREPS; i++) {
        //val = vmin(a, N);
	val = vmin_sse(a, N);
    }
    t = (hpctimer_getwtime() - t) / NREPS;
    printf("Min value: %.4f (real %.4f)\n", val, a[N - 1]);
    printf("Elapsed time: %.6f sec.\n", t);

	_mm_free(a);
    return 0;
}
