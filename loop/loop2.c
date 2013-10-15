/*
 * loop.c:
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hpctimer.h"

enum { n = 64 * 1024 * 1024 };

int main()
{
	int *v, i, sum;
    double t;
	int temp[4] = {0};	
    
	if ( (v = malloc(sizeof(*v) * n)) == NULL) {
		fprintf(stderr, "No enough memory\n");
    	exit(EXIT_FAILURE);
	}
	
	for (i = 0; i < n; i++)
		v[i] = 1;

   	t = hpctimer_wtime();
    for (sum = 0, i = 0; i < n; i+=4) {
	    //sum += v[i];
	    temp[0] += v[i];
	    temp[1] += v[i+1];
	    temp[2] += v[i+2];
	    temp[3] += v[i+3]; 
    }
    sum = temp[0] + temp[1] + temp[2] + temp[3];
    t = hpctimer_wtime() - t;

    printf("Sum = %d\n", sum);
    printf("Elapsed time: %.6f sec.\n", t);
    
	free(v);
	return 0;
}
