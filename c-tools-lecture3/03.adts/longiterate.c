/*
 * longiterate.c: memory leak checking test program for the longhash module.
 *
 * (C) Duncan C. White, 1996-2017 although it seems longer:-)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "longhash.h"



void myPrint( FILE *out, longhashkey k, longhashvalue v )
{
	fprintf( out, "%s: %ld\n", k, v );
}


static longhash h1;


void onerun( void )
{
	h1 = longhashCreate( myPrint );

	longhashSet( h1, "one", 1 );
	longhashSet( h1, "two", 2 );
	longhashSet( h1, "three", 3 );
	longhashSet( h1, "four", 4 );

	longhashSet( h1, "one", 17 );
	longhashSet( h1, "aardvark", 42 );
	longhashSet( h1, "three", 13 );

	longhash h2 = longhashCopy( h1 );

	longhashSet( h1, "one", 18 );
	longhashSet( h2, "one", 11 );

	longhashFree( h1 );
	longhashFree( h2 );
}


int main( int argc, char **argv )
{
	int lim = argc > 1 ? atoi(argv[1]) : 1000;
	int delay = argc > 2 ? atoi(argv[2]) : 0;
	int i;
	printf( "running %d iterations\n", lim );
	for( i=0; i<lim; i++ )
	{
		onerun();
	}
	if( delay > 0 )
	{
		printf( "sleeping for %d seconds\n", delay );
		sleep( delay );
	}
	return 0;
}
