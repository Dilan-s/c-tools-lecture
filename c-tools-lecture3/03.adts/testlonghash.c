/*
 * testhash.c: test program for the longhash module.
 *
 *	       specifically, let's have a hash : string->long
 *
 * (C) Duncan C. White, 1996-2017 although it seems longer:-)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <testutils.h>

#include "longhash.h"



void myPrint( FILE *out, longhashkey k, longhashvalue v )
{
	fprintf( out, "%s: %ld\n", k, v );
}


static longhash h1;


void test( longhashkey k, long expected )
{
	char tstname[1024];
	sprintf( tstname, "test(%s)", k );
	long got = longhashFind( h1, k );
	if( got != -1 )
	{
		if( expected == -1 )
		{
			printf( "T %s: '%s' found when shouldn't be: FAIL\n",
				tstname, k );
		} else if( got == expected )
		{
			printf( "T %s: '%s' found, correct value '%ld': OK\n",
				tstname, k, got );
		} else
		{
			testint( got, expected, tstname );
		}
		//myPrint( stdout, k, got );
	} else
	{
		if( expected == -1 )
		{
			printf( "T %s: '%s' not found: OK\n",
				tstname, k );
		} else
		{
			printf( "T %s: '%s' not found when should be: FAIL\n",
				tstname, k );
		}
	}
}


void lookuptest( int x1, int x2, int x3, int x4, int xaard, int xgrumble )
{
	printf( "lookup tests:\n" );
	test( "one", x1 );
	test( "two", x2 );
	test( "three", x3 );
	test( "four", x4 );
	test( "aardvark", xaard );
	test( "grumble", xgrumble );
}




/* 
 * each_cb: print out a (k,v) pair to the FILE *
 *	    hidden in the arg; used in the main
 *	    program to print the hash ourselves.
 */

static void each_cb( longhashkey k, longhashvalue v, void * arg )
{
	FILE *out = (FILE *)arg;
	myPrint( out, k, v );
}


int main( int argc, char **argv )
{
	h1 = longhashCreate( myPrint );

	if( argc > 1 )
	{
		malloc(strlen(argv[1]));
	}

	longhashSet( h1, "one", 1 );
	longhashSet( h1, "two", 2 );
	longhashSet( h1, "three", 3 );
	longhashSet( h1, "four", 4 );

	printf( "here's the hash:\n" );
	longhashDump( stdout, h1 );

	lookuptest( 1, 2, 3, 4, -1, -1 );

	printf( "modify hash a bit:\n" );
	longhashSet( h1, "one", 3 );
	longhashSet( h1, "aardvark", 17 );
	longhashSet( h1, "three", 30 );
	longhashInc( h1, "three" );
	longhashInc( h1, "four" );
	longhashInc( h1, "five" );

	printf( "now here's the hash:\n" );
	longhashDump( stdout, h1 );

	lookuptest( 3, 2, 31, 5, 17, -1 );

	printf( "\nprint the hash out using an explicit foreach loop:\n{\n" );
	longhashForeach( h1, &each_cb, stdout );
	printf( "}\n" );

	printf( "\ncopy the hash\n" );
	longhash h2 = longhashCopy( h1 );

	printf( "modify both hashes a bit:\n" );
	longhashSet( h1, "one", 18 );
	longhashSet( h2, "one", 11 );

	printf( "print h1:\n" );
	longhashDump( stdout, h1 );

	printf( "print h2:\n" );
	longhashDump( stdout, h2 );

	printf( "free the original hash\n" );
	longhashFree( h1 );

	printf( "print the copy again:\n" );
	longhashDump( stdout, h2 );

	printf( "free the copy\n" );
	longhashFree( h2 );

	return 0;
}
