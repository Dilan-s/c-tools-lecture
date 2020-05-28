/*
 * mintesttree: simple test for automatically generated "tree" data type
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <testutils.h>
#include "datatypes.h"
#include "nleaves.h"


int main( void )
{
	tree t1 = tree_leaf( "absolutely" );
	printf( "1-leaf tree is: " );
	print_tree( stdout, t1 );
	int n = nleaves(t1);
	printf( "\ntree contains %d leaves\n\n", n );
	testint( n, 1, "nleaves(leaf(ab))" );

	tree t2 = tree_leaf( "fabulous" );
	tree t = tree_node( t1, t2 );

	assert( tree_kind(t) == tree_is_node );

	tree l, r;
	get_tree_node( t, &l, &r );
	assert( tree_kind(l) == tree_is_leaf );
	assert( tree_kind(r) == tree_is_leaf );

	assert( nleaves(t) == 2 );

	printf( "2-leaf tree is: " );
	print_tree( stdout, t );
	n = nleaves(t);
	printf( "\ntree contains %d leaves\n\n", n );
	testint( n, 2, "nleaves(node(leaf(ab), leaf(fab))" );

	tree t3 = tree_leaf( "patty" );
	t = tree_node( t, t3 );
	assert( tree_kind(t) == tree_is_node );

	printf( "3-leaf tree is: " );
	print_tree( stdout, t );
	n = nleaves(t);
	printf( "\ntree contains %d leaves\n\n", n );
	testint( n, 3, "nleaves(node(node(leaf(ab), leaf(fab)),leaf(patty))" );

	//free_tree( t );
	return(0);
}
