#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cx.h"


intlist int1, int2, int3;


void test_intlist( void )
{
	printf( "intlists:\n" );
	printf( "\n int1: len %d, ", length(int1) );
	print_intlist( stdout, int1 );
	printf( "\n int2: len %d, ", length(int2) );
	print_intlist( stdout, int2 );
	printf( "\n int3: len %d, ", length(int3) );
	print_intlist( stdout, int3 );
	printf( "\n\n" );
}


void test_illist( void )
{
	illist  il1, il2, il3, il4;

	il1 = illist_nil();
	il2 = illist_cons( int1, illist_nil() );
	il3 = illist_cons( int2, illist_cons( int1, illist_nil() ) );
	il4 = illist_cons( int3, illist_cons( int2, illist_cons( int1, illist_nil() ) ) );

	printf( "illists:\n" );
	printf( "\n il1: len %d, ", lengthil(il1) ); print_illist( stdout, il1 );
	printf( "\n il2: len %d, ", lengthil(il2) ); print_illist( stdout, il2 );
	printf( "\n il3: len %d, ", lengthil(il3) ); print_illist( stdout, il3 );
	printf( "\n il4: len %d, ", lengthil(il4) ); print_illist( stdout, il4 );
	printf( "\n\n" );

	free_illist( il1 );
	free_illist( il2 );
	free_illist( il3 );
	free_illist( il4 );
}


void test_idtree( void )
{
	idtree	id1, id2, id3, id4, id5;

	id1 = idtree_leaf( "hello", int1 );
	id2 = idtree_leaf( "there", int2 );
	id3 = idtree_leaf( "wotcher", int3 );
	id4 = idtree_node( id1, id2 );
	id5 = idtree_node( id3, id4 );

	printf( "idtrees:\n" );
	printf( "\n id1: " ); print_idtree( stdout, id1 );
	printf( "\n id2: " ); print_idtree( stdout, id2 );
	printf( "\n id3: " ); print_idtree( stdout, id3 );
	printf( "\n id4: " ); print_idtree( stdout, id4 );
	printf( "\n id5: " ); print_idtree( stdout, id5 );
	printf( "\n\n" );

	//free_idtree( id1 );
	//free_idtree( id2 );
	//free_idtree( id3 );
	//free_idtree( id4 );
	free_idtree( id5 );
}


int main( void )
{
	init_cx();

	int1 = intlist_nil();
	int2 = intlist_cons( 11, intlist_nil() );
	int3 = intlist_cons( 20, intlist_cons( 11, intlist_nil() ) );

	test_intlist();

	test_illist();

	test_idtree();

	free_intlist( int1 );
	free_intlist( int2 );
	free_intlist( int3 );

	exit(0);
	return 0;
}
