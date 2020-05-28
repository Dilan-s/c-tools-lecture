#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <testutils.h>

#include "strlist.h"


typedef struct { int n, total; } avg;

void avgcb( char *el, void *avgsofar )
{
	int len = strlen(el);
	avg *data = (avg *)avgsofar;
	data->n++;
	data->total += len;
}


int main( void )
{
	strlist l = strlist_nil();
	testbool( strlist_kind( l ) == strlist_is_nil,
		"kind(nil) == nil" );

	l = strlist_cons( "duncan", l );

	testbool( strlist_kind( l ) == strlist_is_cons,
		"kind([duncan]) == cons" );

	strlist tail;
	char *head;
	get_strlist_cons( l, &head, &tail );
	teststring( head, "duncan", "head([duncan]) == duncan" );
	testbool( strlist_kind( tail ) == strlist_is_nil,
		"kind(tail([duncan])) == nil" );

	l = strlist_cons( "hi", l );
	get_strlist_cons( l, &head, &tail );
	teststring( head, "hi", "head([hi,duncan]) == hi" );
	get_strlist_cons( tail, &head, &tail );
	teststring( head, "duncan", "head(tail([hi,duncan])) == duncan" );

	l = strlist_cons( "x", l );
	l = strlist_cons( "y", l );

	char result[1000];
	sprint_strlist( result, l );
	teststring( result, "[ y,x,hi,duncan ]", "sprint test" );

	avg av;
	av.n     = 0;
	av.total = 0;
	foreach_strlist( avgcb, (void *)&av, l );
	testbool( av.n == 4, "len list == 4" );
	testbool( av.total == 10, "total list == 10" );

	free_strlist( l );

	return(0);
}
