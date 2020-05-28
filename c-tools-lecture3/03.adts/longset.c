/*
 * longset.c: set of longs (based on hashes) storage for C..
 *
 * (C) Duncan C. White, 1996-2019 although it seems longer:-)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "longset.h"


#define	NHASH	32533


typedef struct tree_s *tree;


struct longset_s {
	tree *			data;
	longsetprintfunc	p;
	int			nmembers;
};

struct tree_s {
	long		k;			/* key aka set member */
	bool		in;			/* in, i.e. not deleted */
	tree		left;			/* Left... */
	tree		right;			/* ... and Right ptr's */
};


/*
 * operation
 */
typedef enum { Search, Define, Exclude } ops;


/* Private functions */

static void adddelop( long k, void * v );
static void exclude_if_notin_cb( long k, void * arg );
static void diff_cb( long k, void * arg );
static void dump_foreachcb( long k, void * arg );
static tree talloc( long k );
static int shash( long l );
static tree symop( longset s, long k, ops op );
static void foreach_tree( tree t, longsetforeachcb f, void * arg );
static tree copy_tree( tree t );
static void free_tree( tree t );
static int depth_tree( tree t );


/*
 * Create an empty longset
 */
longset longsetCreate( longsetprintfunc p )
{
	longset   s = (longset) malloc( sizeof(struct longset_s) );
	s->data = (tree *) malloc( NHASH*sizeof(tree) );
	s->p = p;
	s->nmembers = 0;

	int   i;
	for( i = 0; i < NHASH; i++ )
	{
		s->data[i] = NULL;
	}
	return s;
}


/*
 * Empty an existing longset - ie. retain only the skeleton..
 */
void longsetEmpty( longset s )
{
	int   i;

	for( i = 0; i < NHASH; i++ )
	{
		free_tree( s->data[i] );
		s->data[i] = NULL;
	}
	s->nmembers = 0;
}


/*
 * Copy an existing longset.
 */
longset longsetCopy( longset s )
{
	int   i;
	longset   result;

	result = (longset) malloc( sizeof(struct longset_s) );
	result->data = (tree *) malloc( NHASH*sizeof(tree) );
	result->p = s->p;
	result->nmembers = s->nmembers;

	for( i = 0; i < NHASH; i++ )
	{
		result->data[i] = copy_tree( s->data[i] );
	}

	return result;
}


/*
 * Free the given longset - clean it up and delete it's skeleton too..
 */
void longsetFree( longset s )
{
	int   i;

	for( i = 0; i < NHASH; i++ )
	{
		free_tree( s->data[i] );
	}
	free( (void *) s->data );
	free( (void *) s );
}


/*
 * longset metrics:
 *  calculate the min, max and average depth of all non-empty trees
 *  sadly can't do this with a longsetForeach unless the depth is magically
 *  passed into the callback..
 */
void longsetMetrics( longset s, int *min, int *max, double *avg )
{
	int	i;
	int	nonempty = 0;
	int	total    = 0;

	*min =  100000000;
	*max = -100000000;
	for( i = 0; i < NHASH; i++ ) {
		if( s->data[i] != NULL )
		{
			int d = depth_tree( s->data[i] );
			if( d < *min ) *min = d;
			if( d > *max ) *max = d;
			total += d;
			nonempty++;
		}
	}
	*avg = ((double)total)/(double)nonempty;
}


/*
 * Add k to the longset s
 */
void longsetAdd( longset s, long k )
{
	(void) symop( s, k, Define);
}


/*
 * Remove k from the longset s
 */
void longsetRemove( longset s, long k )
{
	(void) symop( s, k, Exclude);
}


/*
 * Convenience function:
 *  Given a changes string of the form "[+-]item[+-]item[+-]item..."
 *  modify the given longset s, including (+) or excluding (-) items
 * NB: This assumes that key == char *..
 */
void longsetModify( longset s, char *changes )
{
	char *str = strdup( changes );	/* so we can modify it! */
	char *p = str;

	char cmd = *p;
	while( cmd != '\0' )		/* while not finished */
	{
		assert( cmd == '+' || cmd == '-' );
		p++;

		/* got a string of the form... [+-]itemstring[+-\0]... */
		/* cmd = the + or - command                            */
		/* and p points at the first char  ^p                  */

		/* find the next +- command,                  ^q       */
		char *q = p;
		for( ; *q != '\0' && *q != '+' && *q != '-'; q++ );

		/* terminate itemstring here, remembering the next cmd */
		char nextcmd = *q;
		*q = '\0';

		long item = atol( p );

		/* now actually include/exclude the item from the longset  */
		if( cmd == '+' )
		{
			longsetAdd( s, item );
		} else
		{
			longsetRemove( s, item );
		}

		/* set up for next time                                */
		cmd = nextcmd;			   /* the next command */
		p = q;				   /* the next item    */
	}
	free( (void *)str );
}


/*
 * Look for something in the longset s
 */
bool longsetIn( longset s, long k )
{
	tree x = symop(s, k, Search);

	return x != NULL && x->in;
}


/*
 * perform a foreach operation over a given longset
 * call a given callback for each item pair.
 */
void longsetForeach( longset s, longsetforeachcb cb, void * arg )
{
	int	i;

	for( i = 0; i < NHASH; i++ ) {
		foreach_tree( s->data[i], cb, arg );
	}
}


typedef enum { Uncond, IfIn, IfNotIn } cond;

typedef struct
{
	cond      c;
	longset	  result;
	longset	  other;
	int       add;
} setop;


static void adddelop( long k, void *v )
{
	setop *d = (setop *)v;
	if( d->c == Uncond
	|| (d->c == IfIn && longsetIn(d->other,k))
	|| (d->c == IfNotIn && ! longsetIn(d->other,k))
	) {
		if( d->add )
		{
			longsetAdd( d->result, k );
		} else
		{
			longsetRemove( d->result, k );
		}
	}
}


/*
 * Set union, a += b
 */
void longsetUnion( longset a, longset b )
{
	setop       data;
	data.result = a;
	data.c      = Uncond;
	data.add    = 1;

	longsetForeach( b, &adddelop, (void *)&data );
}


/*
 * long set intersection, a = a&b
 *   exclude each member of a FROM a UNLESS in b too
 *   here we need to pass both sets to the callback,
 *   via this "pair of sets" structure:
 */
typedef struct { longset a, b; } setpair;
static void exclude_if_notin_cb( long k, void *arg )
{
	setpair *d = (setpair *)arg;
	if( ! longsetIn(d->b, k) )
	{
		longsetRemove( d->a, k );
	}
}

void longsetIntersection( longset a, longset b )
{
	setpair data; data.a = a; data.b = b;
	longsetForeach( a, &exclude_if_notin_cb, (void *)&data );
}


/*
 * Set difference, simultaneous a -= b and b -= a
 *  exclude each item of both sets from both sets, LEAVING
 *  - a containing elements ONLY in a, and
 *  - b containing elements ONLY in b.
 */
static void diff_cb( long k, void *arg )
{
	setpair *d = (setpair *)arg;
	if( longsetIn(d->a, k) )
	{
		longsetRemove( d->a, k );
		longsetRemove( d->b, k );
	}
}


void longsetDiff( longset a, longset b )
{
	setpair data; data.a = a; data.b = b;
	longsetForeach( b, &diff_cb, (void *)&data );
}


/*
 * longsetNMembers: how many members in the set?
 */
//static void count_cb( long k, void *arg )
//{
//	int *n = (int *)arg;
//	(*n)++;
//}
int longsetNMembers( longset s )
{
	//int n = 0;
	//longsetForeach( s, &count_cb, (void *)&n );
	//return n;
	return s->nmembers;
}

/*
 * Set is empty? Is the set empty?
 */
bool longsetIsEmpty( longset s )
{
	//return longsetNMembers( s ) == 0;
	return s->nmembers == 0;
}

/*
 * Set subtraction, a -= b
 */
void longsetSubtraction( longset a, longset b )
{
	setop       data;
	data.result = a;
	data.other  = a;
	data.c      = IfIn;
	data.add    = 0;

	longsetForeach( b, &adddelop, (void *)&data );
}


/*
 * Display a given set - print each item
 * by calling the array's printfunc
 */
typedef struct {
	FILE *out;
	longsetprintfunc p;
} dumpdata;


void longsetDump( FILE *out, longset s )
{
	dumpdata arg;

	arg.p = s->p;
	arg.out = out;

	//fputc('\n',out);
	longsetForeach( s, &dump_foreachcb, (void *)&arg );
	//fputc('\n',out);
}


/*
 * dump foreach callback:
 *	print a single item to (dumpdata *)arg
 */
static void dump_foreachcb( long k, void * arg )
{
	dumpdata *dd = (dumpdata *)arg;

	if( dd->p != NULL )
	{
		(*(dd->p))( dd->out, k );
	} else
	{
		//fprintf( dd->out, "%20s\n", k );
		fprintf( dd->out, "%ld,", k );
	}
}


/*
 * Allocate a new node in the tree
 */
static tree talloc( long k )
{
	tree   p = (tree) malloc(sizeof(struct tree_s));

	if( p == NULL )
	{
		fprintf( stderr, "talloc: No space left\n" );
		exit(1);
	}
	p->left = p->right = NULL;
	p->k    = k;
	p->in   = true;			/* Include it */
	return p;
}


/*
 * Calculate hash on a long
 */
static int shash( long l )
{
	unsigned long	hh;

	for (hh = 0; l; hh = hh * 65599 + (l%10), l /= 10 )
	{
	}
	return hh % NHASH;
}


/*
 * Operate on the symbol table
 * Search, Define, Exclude.
 */
static tree symop( longset s, long k, ops op )
{
	tree	ptr;
	tree *	aptr = s->data + shash(k);

	while( (ptr = *aptr) != NULL )
	{
		if( ptr->k == k )
		{
			if( op == Define )
			{
				if( ! ptr->in )
				{
					s->nmembers++;
					ptr->in = true;
				}
			} else if( op == Exclude )
			{
				if( ptr->in )
				{
					ptr->in = false;
					s->nmembers--;
				}
			} else if( ! ptr->in )
			{
				return NULL;
			}
			return ptr;
		}
		if( ptr->k < k )
		{
			/* less - left */
			aptr = &(ptr->left);
		} else
		{
			/* more - right */
			aptr = &(ptr->right);
		}
	}

	if (op == Define )
	{
		ptr = *aptr = talloc(k);	/* Alloc new node */
		s->nmembers++;
		return ptr;
	}

	return NULL;				/* not found */
}


/*
 * foreach one tree
 */
static void foreach_tree( tree t, longsetforeachcb f, void * arg )
{
	assert( f != NULL );
	if( t )
	{
		foreach_tree( t->left, f, arg );
		if( t->in )
		{
			(*f)( t->k, arg );
		}
		foreach_tree( t->right, f, arg );
	}
}


/*
 * Copy one tree
 */
static tree copy_tree( tree t )
{
	tree result = NULL;
	if( t )
	{
		result = talloc( t->k );
		result->in    = t->in;
		result->left  = copy_tree( t->left );
		result->right = copy_tree( t->right );
	}
	return result;
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
