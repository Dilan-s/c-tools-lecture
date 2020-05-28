#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <testutils.h>

#include "wordlist.h"


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
	wordlist l = wordlist_nil();
	testbool( wordlist_kind( l ) == wordlist_is_nil,
		"kind(nil) == nil" );

	l = wordlist_cons( "duncan", l );
	testbool( wordlist_kind( l ) == wordlist_is_cons,
		"kind([duncan]) == cons" );

	wordlist tail;
	wordlistelement head;
	get_wordlist_cons( l, head, &tail );
	teststring( head, "duncan", "head([duncan]) == duncan" );
	testbool( wordlist_kind( tail ) == wordlist_is_nil,
		"kind(tail([duncan])) == nil" );

	l = wordlist_cons( "hi", l );
	get_wordlist_cons( l, head, &tail );
	teststring( head, "hi", "head([hi,duncan]) == hi" );
	get_wordlist_cons( tail, head, &tail );
	teststring( head, "duncan", "head(tail([hi,duncan])) == duncan" );

	l = wordlist_cons( "x", l );
	l = wordlist_cons( "y", l );

	char result[1000];
	sprint_wordlist( result, l );
	teststring( result, "[ y,x,hi,duncan ]", "sprint test" );

	avg av;
	av.n     = 0;
	av.total = 0;
	foreach_wordlist( avgcb, (void *)&av, l );
	testbool( av.n == 4, "len list == 4" );
	testbool( av.total == 10, "total list == 10" );

	free_wordlist( l );

	return 0;
}
