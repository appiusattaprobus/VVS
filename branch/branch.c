/*
 * branch.c:
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hpctimer.h"

enum { 
    n = 100000,
    nreps = 20 
};

double x[n], y[n], z[n];

void blend_map(double *dest, double *a, double *b, int size, int blend)
{
    int i = 0;
    
    if ( blend == 255 )
    	for (i = 0; i < size; i++)
    		dest[i] = a[i];
    else if ( blend == 0 )
	for (i = 0; i < size; i++)
		dest[i] = b[i];
    else 
	for (i = 0; i < size; i++)
		dest[i] = a[i] * blend + b[i] * (255 - blend) / 256.0;
    
}

int main()
{
    double t;
    int i;
        
   	t = hpctimer_wtime();
    for (i = 0; i < nreps; i++) {
        blend_map(z, x, y, n, 0);
    }
    t = (hpctimer_wtime() - t) / nreps;

    printf("Elapsed time: %.6f sec.\n", t);
    
	return 0;
}
