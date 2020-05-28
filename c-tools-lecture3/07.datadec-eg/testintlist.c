/*
 * test automatically generated "intlist" data type
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <testutils.h>
#include "datatypes.h"


int main( void )
{
	intlist l = intlist_nil();
	testbool( intlist_kind( l ) == intlist_is_nil,
		"kind(nil) == nil" );

	l = intlist_cons( 100, l );

	testbool( intlist_kind( l ) == intlist_is_cons,
		"kind([100]) == cons" );

	intlist tail;
	int head;
	get_intlist_cons( l, &head, &tail );
	testint( head, 100, "head([100]) == 100" );
	testbool( intlist_kind( tail ) == intlist_is_nil,
		"kind(tail([100])) == nil" );

	l = intlist_cons( 200, l );
	get_intlist_cons( l, &head, &tail );
	testint( head, 200, "head([200,100]) == 200" );
	get_intlist_cons( tail, &head, &tail );
	testint( head, 100, "head(tail([200,100])) == 100" );

	l = intlist_cons( 300, l );
	l = intlist_cons( 400, l );

	int sum = 0;
	intlist p = l;
	while( intlist_kind(p) == intlist_is_cons )
	{
		int head;
		intlist tail;
		get_intlist_cons( p, &head, &tail );
		sum += head;
		p = tail;
	}
	testint( sum, 1000, "sum 1000" );

	return(0);
}
