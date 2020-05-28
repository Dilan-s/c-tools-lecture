/*
 * testlongset.c: test program for the hash-based longset module.
 *
 * (C) Duncan C. White, 1996-2019 although it seems longer:-)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "longset.h"



static longset s;


void test( long k, bool expected )
{
	char tstname[1024];
	sprintf( tstname, "test(%ld)", k );
	if( longsetIn( s, k ) )
	{
		if( ! expected )
		{
			printf( "T %s: '%ld' in when shouldn't be: FAIL\n",
				tstname, k );
		} else
		{
			printf( "T %s: '%ld' in: OK\n", tstname, k );
		}
	} else
	{
		if( expected )
		{
			printf( "T %s: '%ld' not in when it should be\n",
				tstname, k );
		} else
		{
			printf( "T %s: '%ld' not in: OK\n", tstname, k );
		}
	}
}


void lookuptest( int x1, int x2, int x3, int x4, int xaard, int xgrumble )
{
	printf( "lookup tests:\n" );
	test( 17, x1 );
	test( -325L, x2 );
	test( 200, x3 );
	test( 32768, x4 );
	test( 28, xaard );
	test( 45L, xgrumble );
}


static void myPrint( FILE *out, long k )
{
	fprintf( out, "%ld,", k );
}


/* 
 * each_cb: print out an item to the FILE *
 *	    hidden in the arg; used in the main
 *	    program to print the hash ourselves.
 */

static void each_cb( long k, void *arg )
{
	FILE *out = (FILE *)arg;
	myPrint( out, k );
}


int main( int argc, char **argv )
{
	s = longsetCreate( myPrint );

	if( argc > 1 )
	{
		malloc(strlen(argv[1]));
	}

	longsetAdd( s, 17 );
	longsetAdd( s, -325L );
	longsetAdd( s, 200 );
	longsetAdd( s, 32768 );

	printf( "here's the longset:\n" );
	longsetDump( stdout, s );

	lookuptest( 1, 1, 1, 1, 0, 0 );

	printf( "modify longset a bit:\n" );
	longsetRemove( s, -325L );
	longsetAdd( s, 201 );
	longsetAdd( s, 28 );

	printf( "now here's the longset:\n" );
	longsetDump( stdout, s );

	lookuptest( 1, 0, 1, 1, 1, 0 );

	printf( "\nprint the longset out using an explicit foreach loop:\n{" );
	longsetForeach( s, &each_cb, stdout );
	printf( "}\n" );

	printf( "\nfree the longset\n" );
	longsetFree( s );

	return 0;
}
