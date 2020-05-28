/*
 * wordset.c: wordtest program for the hash-based set module.
 *
 * (C) Duncan C. White, 2008 new addition to the fold..
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "set.h"


static void myPrint( FILE *out, setkey k )
{
	fprintf( out, "%s,", k );
}


int main( int argc, char **argv )
{
	set s1 = setCreate( myPrint );

	if( argc > 1 )
	{
		malloc(strlen(argv[1])); /* and don't free it! */
	}

	FILE *in = fopen( "/usr/share/dict/words", "r" );
	if( in == NULL )
	{
		in = fopen( "/usr/dict/words", "r" );	/* trad */
	}
	assert( in != NULL );

	#define MAXWD	200
	char word[MAXWD];

	while( fgets(word,MAXWD,in) != NULL )
	{
		word[strlen(word)-1] = '\0';
		setAdd( s1, word );
	}
	fclose( in );

	int mind, maxd;
	double avg;
	setMetrics( s1, &mind, &maxd, &avg );
	printf( "n(s1)=%d, mindepth(s1)=%d, maxdepth(s1)=%d, average depth(s1)=%.2f\n",
		setNMembers(s1), mind, maxd, avg );

	printf( "s2 = s1\n" );
	set s2 = setCopy( s1 );
	char *changes = "-aardvark+boomboom+neverneverland";
	printf( "s2 = s2 %s\n", changes );
	setModify( s2, changes );
	changes = "+widdershins-clockwise";
	printf( "s1 = s1 %s\n", changes );
	setModify( s1, changes );

	printf( "n(s2)=%d\n", setNMembers(s2) );

	printf( "\nafter setdiff, " );
	setDiff( s1, s2 );

	printf( "results are:\n\n" );
	printf( "s1=" ); setDump( stdout, s1 ); printf( "\n" );
	printf( "s2=" ); setDump( stdout, s2 ); printf( "\n" );

	setFree( s1 );
	setFree( s2 );

	return 0;
}
