#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// setting the number of threads:
#ifndef NUMT
#define NUMT		1
#endif

#ifndef NUMNODES
#define NUMNODES    32
#endif

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	10
#endif

#define N   4

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

float Height( int, int );


int main( int argc, char *argv[ ] )
{

    omp_set_num_threads( NUMT );

    float maxPerformance = 0;
    float totalVolume = 0;

	// the area of a single full-sized tile:

    for (int t = 0; t < NUMTRIES; t++){
        
        double time0 = omp_get_wtime( );

        float volume = 0;
        float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
                    ( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );

        // sum up the weighted heights into the variable "volume"
        // using an OpenMP for loop and a reduction:
        #pragma omp parallel for default(none), shared(fullTileArea),reduction(+:volume)
            for( int i = 0; i < NUMNODES*NUMNODES; i++ )
            {
                int iu = i % NUMNODES;
                int iv = i / NUMNODES;
                float z = Height( iu, iv );
                float area;

                // This is for the corners
                if((iu == 0 || iu == NUMNODES - 1) && (iv == 0 || iv == NUMNODES - 1)){
                    area = fullTileArea / 4;
                }
                // This is for the edges
                else if(iu == 0 || iu == NUMNODES -1 || iv == 0 || iv == NUMNODES -1){
                    area = fullTileArea / 2;
                }
                // The full tile area
                else{
                    area = fullTileArea;
                }

                volume += (area * z);
            }

         float tempVolume = volume * 2;

        // printf("Total Volume: %f\n", totalVolume);

        double time1 = omp_get_wtime( );
		double megaHeightsPerSecond = (double)(NUMNODES*NUMNODES)/ ( time1 - time0 ) / 1000000.;
		if( megaHeightsPerSecond > maxPerformance )
			maxPerformance = megaHeightsPerSecond;

        if(tempVolume > totalVolume)
            totalVolume = tempVolume;
    }

        printf("%d  %8.2lf  %f\n", NUMT,maxPerformance, totalVolume);

}

float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float r = 1. - xn - yn;
	if( r < 0. )
	        return 0.;
	float height = pow( 1. - xn - yn, 1./(float)N );
	return height;
}