#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csvsplit.h"


static void testint( int v, int expected, char *msg )
{
	if( v != expected )
	{
		printf( "T %s: should be %d, was %d: bad\n",
			msg, expected, v );
	} else
	{
		printf( "T %s: is %d: ok\n", msg, v );
	}
}


static void teststr( char * v, char * expected, char *msg )
{
	if( strcmp( v, expected ) != 0 )
	{
		printf( "T %s: should be %s, was %s: bad\n",
			msg, expected, v );
	} else
	{
		printf( "T %s: is %s: ok\n", msg, v );
	}
}


// counting tests: tests of counting how many csv values a csv-string contains:
#define MAXFIELDS 5
typedef struct { char *csv; int nfields; char *value[MAXFIELDS]; } counttest;

counttest counttests[] = {
	{ "hello", 1, { "hello" } },
	{ "a,b",   2, { "a", "b" } },
	{ "a,",    2, { "a", "" } },
	{ "a,b,c", 3, { "a", "b", "c" } },
	{ "",      1, { "" } },
	{ NULL,    0, { NULL } },
};


static void count_callback( char *element, void *extra )
{
	int *np = (int *) extra;
	(*np)++;
}


static void countfields( char *csv, int *np )
{
	csvForeach( csv, &count_callback, (void *)np );
}


typedef struct { counttest *ct; int nthcall; } value_extra;

static void value_callback( char *element, void *extra )
{
	value_extra *ve = (value_extra *)extra;

	char *sp = ve->ct->value[ve->nthcall];

	ve->nthcall++;

	if( ve->nthcall > ve->ct->nfields )
	{
		printf( "T fields(%s): too many value callbacks, "
			"%d fields, call %d: bad\n",
			ve->ct->csv, ve->ct->nfields, ve->nthcall );
	}

	char msg[1024];
	sprintf( msg, "field(%d) of %s", ve->nthcall, ve->ct->csv );
	teststr( element, sp, msg );
}


static void testvalues(counttest *ct)
{
	value_extra ve; ve.ct = ct; ve.nthcall = 0;
	csvForeach( ct->csv, &value_callback, (void *)&ve );
}


int main(void)
{
	int i;
	for( i=0; counttests[i].csv != NULL; i++ )
	{
		counttest *ct = &counttests[i];
		char msg[1024];
		sprintf( msg, "countfields(%s)=%d",
			ct->csv, ct->nfields );
		int n=0;
		countfields(ct->csv,&n);
		testint( n, ct->nfields, msg );
	}
	for( i=0; counttests[i].csv != NULL; i++ )
	{
		counttest *ct = &counttests[i];
		testvalues(ct);
	}
	return 0;
}
