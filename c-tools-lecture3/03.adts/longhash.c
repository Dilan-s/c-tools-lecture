/*
 * longhash.c: hash storage for C where the values are unsigned longs..
 *	   modified version of hash.c
 * 	   we store a hash as a hash table, hashing each longhashkey into
 * 	   a dynamic array of binary search trees, and then
 * 	   search/include/exclude the longhashkey,longhashvalue pair in/from the
 * 	   corresponding search tree.  The hash also stores a func
 * 	   ptrs: a (file,longhashkey,longhashvalue) print function.
 *
 * (C) Duncan C. White, 1996-2017 although it seems longer:-)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "longhash.h"


#define	NHASH	32533


typedef struct tree_s *tree;


struct longhash_s {
	tree *			data;		/* dynamic array of trees */
	longhashprintfunc	p;		/* how to print (k,v) pair */
};

struct tree_s {
	longhashkey	k;			/* longhashkey */
	longhashvalue   v;			/* longhashvalue */
	tree		left;			/* Left... */
	tree		right;			/* ... and Right trees */
};


/*
 * operation
 */
typedef enum { Search, Define } tree_operation;


/* Private functions */

static tree copy_tree( tree );
static void foreach_tree( tree, longhashforeachcb, void * );
static void dump_cb( longhashkey, longhashvalue, void * );
static void free_tree( tree );
static int depth_tree( tree );
static long tree_op( longhash, longhashkey, longhashvalue, tree_operation );
static tree talloc( longhashkey, longhashvalue );
static int shash( char * );


/*
 * Create an empty hash
 */
longhash longhashCreate( longhashprintfunc p )
{
	int  i;
	longhash h;

	h = (longhash) malloc( sizeof(struct longhash_s) );

	h->data = (tree *) malloc( NHASH*sizeof(tree) );

	h->p = p;

	for( i = 0; i < NHASH; i++ )
	{
		h->data[i] = NULL;
	}

	return h;
}


/*
 * Empty an existing longhash - ie. retain only the skeleton..
 */
void longhashEmpty( longhash a )
{
	int   i;

	for( i = 0; i < NHASH; i++ )
	{
		if( a->data[i] != NULL )
		{
			free_tree( a->data[i] );
			a->data[i] = NULL;
		}
	}
}


/*
 * Copy an existing longhash, including copying the values
 */
longhash longhashCopy( longhash h )
{
	int   i;
	longhash   result;

	result = (longhash) malloc( sizeof(struct longhash_s) );
	assert( result != NULL );
	result->data = (tree *) malloc( NHASH*sizeof(tree) );
	assert( result->data != NULL );
	result->p = h->p;

	for( i = 0; i < NHASH; i++ )
	{
		result->data[i] = NULL;
		if( h->data[i] != NULL )
		{
			result->data[i] = copy_tree( h->data[i] );
		}
	}

	return result;
}


/*
 * Free the given longhash - clean it up and delete it's skeleton too..
 */
void longhashFree( longhash h )
{
	int   i;

	for( i = 0; i < NHASH; i++ )
	{
		if( h->data[i] != NULL )
		{
			free_tree( h->data[i] );
		}
	}

	free( (void *) h->data );
	free( (void *) h );
}


/*
 * Add k->v to the longhash h
 */
void longhashSet( longhash h, longhashkey k, longhashvalue v )
{
	(void) tree_op( h, k, v, Define);
}


/*
 * Look for key k in the longhash h;
 * return -1 if not found
 */
long longhashFind( longhash h, longhashkey k )
{
	long l = tree_op(h, k, 0, Search);
	return l;
}


/*
 * Increment the value associated with key k in longhash h
 * (adding it if absent, like auto-vivify in Perl)
 */
void longhashInc( longhash h, longhashkey k )
{
	long l = tree_op(h, k, 0, Search);
	if( l == -1 ) l = 0;
	l++;
	(void) tree_op( h, k, l, Define);
}


/*
 * perform a foreach operation over a given longhash
 * call a given callback for each (name, longhashvalue) pair.
 */
void longhashForeach( longhash h, longhashforeachcb cb, void * arg )
{
	int  i;
	for( i = 0; i < NHASH; i++ ) {
		if( h->data[i] != NULL )
		{
			foreach_tree( h->data[i], cb, arg );
		}
	}
}


/* ----------- Higher level operations using setForeach -------------- */
/* - each using it's own callback, sometimes with a custom structure - */


/*
 * longhashDump: Display a given longhash - print each name and longhashvalue
 *  by calling the longhash's printfunc (or a default if NULL)
 *  Here, we need to know where (FILE *out) and how (printfunc p) to
 *  display each item of the set.
 */
typedef struct { FILE *out; longhashprintfunc p; } dumparg;
static void dump_cb( longhashkey k, longhashvalue v, void * arg )
{
	dumparg *dd = (dumparg *)arg;
	if( dd->p != NULL )
	{
		(*(dd->p))( dd->out, k, v );
	} else
	{
		fprintf( dd->out, "%20s -> %08lx\n", k, v );
	}
}


void longhashDump( FILE *out, longhash h )
{
	dumparg arg;
	arg.p   = h->p;
	arg.out = out;

	fputc('\n',out);
	longhashForeach( h, &dump_cb, (void *)&arg );
	fputc('\n',out);
}


/*
 * Allocate a new node in the tree
 */
static tree talloc( longhashkey k, longhashvalue v )
{
	tree   p = (tree) malloc(sizeof(struct tree_s));

	if( p == NULL )
	{
		fprintf( stderr, "talloc: No space left\n" );
		exit(1);
	}
	p->left = p->right = NULL;
	p->k = strdup(k);		/* Save longhashkey */
	p->v = v;			/* longhashvalue */
	return p;
}


/*
 * longhash metrics:
 *  calculate the min, max and average depth of all non-empty trees
 *  sadly can't do this with a longhashForeach unless the depth is magically
 *  passed into the callback..
 */
void longhashMetrics( longhash h, int *min, int *max, double *avg )
{
	int	i;
	int	nonempty = 0;
	int	total    = 0;

	*min =  100000000;
	*max = -100000000;
	for( i = 0; i < NHASH; i++ ) {
		if( h->data[i] != NULL )
		{
			int d = depth_tree( h->data[i] );
			if( d < *min ) *min = d;
			if( d > *max ) *max = d;
			total += d;
			nonempty++;
		}
	}
	*avg = ((double)total)/(double)nonempty;
}


/*
 * Operate on the binary search tree
 * Search, Define.
 */
static long tree_op( longhash h, longhashkey k, longhashvalue v, tree_operation op )
{
	tree	ptr;
	tree *	aptr = h->data + shash(k);

	while( (ptr = *aptr) != NULL )
	{
		int rc = strcmp(ptr->k, k);
		if( rc == 0 )
		{
			if (op == Define)
			{
				/* set new value */
				ptr->v = v;
			}
			return ptr->v;
		}
		if (rc < 0)
		{
			/* less - left */
			aptr = &(ptr->left);
		} else
		{
			/* more - right */
			aptr = &(ptr->right);
		}
	}

	if (op == Define)
	{
		*aptr = talloc(k,v);	/* Alloc new node */
		return v;
	}

	return -1;			/* not found */
}


/*
 * Copy one tree
 */
static tree copy_tree( tree t )
{
	tree result = NULL;
	if( t )
	{
		result = talloc( t->k, t->v );
		result->left  = copy_tree( t->left );
		result->right = copy_tree( t->right );
	}
	return result;
}


/*
 * foreach one tree
 */
static void foreach_tree( tree t, longhashforeachcb f, void * arg )
{
	assert( f != NULL );

	if( t )
	{
		foreach_tree( t->left, f, arg );
		(*f)( t->k, t->v, arg );
		foreach_tree( t->right, f, arg );
	}
}


/*
 * Free one tree
 */
static void free_tree( tree t )
{
	if( t )
	{
		free_tree( t->left );
		free_tree( t->right );
		free( (void *) t->k );
		free( (void *) t );
	}
}


/*
 * Compute the depth of a given tree
 */
#define max(a,b)  ((a)>(b)?(a):(b))
static int depth_tree( tree t )
{
	if( t )
	{
		int d2 = depth_tree( t->left );
		int d3 = depth_tree( t->right );
		return 1 + max(d2,d3);
	}
	return 0;
}


/*
 * Calculate hash on a string
 */
static int shash( char *str )
{
	unsigned char	ch;
	unsigned int	hh;
	for (hh = 0; (ch = *str++) != '\0'; hh = hh * 65599 + ch );
	return hh % NHASH;
}
