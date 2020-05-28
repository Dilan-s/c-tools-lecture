/*
 * nleaves: count leaves in automatically generated "tree" data type
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "datatypes.h"
#include "nleaves.h"


/* recursive tree leaf counter
 * In Haskell, this'd be:
 *   nleaves(leaf(name))   = 1
 *   nleaves(node(l,r)) = nleaves(l) + nleaves(r)
 */
int nleaves( tree t )
{
	if( tree_kind(t) == tree_is_leaf )     	// a leaf
	{
		string name; get_tree_leaf( t, &name );
		// leaf( name ): contains 1 leaf
		return 1;
	} else						// a node
	{
		tree l, r; get_tree_node( t, &l, &r );
		// node( l, r ): process l and r trees:
		return nleaves(l) + nleaves(r);
	}
}
