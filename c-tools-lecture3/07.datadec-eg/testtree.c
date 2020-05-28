/*
 * test automatically generated "tree" data types
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <testutils.h>
#include "datatypes.h"
#include "nleaves.h"


void testleaf( tree t, char *expected, char *treename )
{
	char label[1024];
	sprintf( label, "isnode(%s)", treename );
	testbool( tree_kind(t) == tree_is_leaf, label );
	string name;
	get_tree_leaf( t, &name );
	sprintf( label, "getleaf(%s)", treename );
	teststring( name, expected, label );
}


/*
 * append value onto the end of malloced chunk *result, maxlen max,
 * current length *len, updating the length.  the value must fit in!
 */
void append_mustfit( char *result, int max, int *len, char *value )
{
	int vlen = strlen(value)+1;
	assert( vlen + *len < max );
	strcat( result, value );
	*len += vlen;
}


/* recursive leaf concatenator,
 * storage allocation makes this damn tricky
 */
void ccleaves_recrsv( tree t, char *sep, char *result, int max, int *len )
{
	if( tree_kind(t) == tree_is_leaf )
	{
		string name;
		get_tree_leaf( t, &name );
		append_mustfit( result, max, len, name );
		append_mustfit( result, max, len, sep );
	} else
	{
		tree l, r;
		get_tree_node( t, &l, &r );
		ccleaves_recrsv( l, sep, result, max, len );
		ccleaves_recrsv( r, sep, result, max, len );
	}
}


/*
 * tree leaf concatenator, with a separator (sep) between the leaves
 * returns a malloc()ed string, caller is responsible for freeing it.
 */
string concatleaves( tree t, char *sep )
{
	int max = 1024;
	char *result = (char *) malloc( max * sizeof(char) );
	int currlen = 0;
	*result = '\0';
	ccleaves_recrsv( t, sep, result, max, &currlen );
	return result;
}


int main( void )
{
	tree t1 = tree_leaf( "absolutely" );
	testleaf( t1, "absolutely", "ab" );

	tree t2 = tree_leaf( "fabulous" );
	testleaf( t2, "fabulous", "fab" );

	tree t = tree_node( t1, t2 );

	testbool( tree_kind(t) == tree_is_node, "isnode((ab,fab))" );

	tree l, r;
	get_tree_node( t, &l, &r );

	testleaf( l, "absolutely", "left((ab,fab))" );
	testleaf( r, "fabulous", "right((ab,fab))" );

	testint( nleaves(t), 2, "nleaves((ab,fab))" );

	tree t3 = tree_leaf( "patty" );
	testleaf( t3, "patty", "pat" );

	t = tree_node( t, t3 );
	testbool( tree_kind(t) == tree_is_node,
		"isnode(((ab,fab),pat))" );
	testint( nleaves(t), 3, "nleaves(((ab,fab),pat))" );

	string s = concatleaves( t, "," );
	teststring( s, "absolutely,fabulous,patty,",
		"concatleaves(((ab,fab),pat),',')" );
	free(s);
	//free_tree(t);

	return(0);
}
