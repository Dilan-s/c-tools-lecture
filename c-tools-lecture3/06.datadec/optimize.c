/* optimize.c */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "struct.h"
#include "optimize.h"


bool opt;		/* opt      == perform optimizations    */
bool verbose;		/* verbose  == be verbose - diagnostics */
bool metaonly;		/* metaonly == ONLY generate the meta-data on output */


static void optimize_decln( decln );
static bool tail_optimize( decln );


#define implies( a, b ) (!(a) || (b))


void optimize( declnlist d )
{
	for( ; d != NULL; d = d->next )
	{
		if( opt )
		{
			optimize_decln( d );
		} else
		{
			d->ManyShapes = d->TagField = d->Struct = d->Union = true;
			d->UseNull = d->PutLoop = false;
		}
	}
}


static void optimize_decln( decln d )
{
	int		t, e, ne;
	shapelist	s;
	bool		firstempty = d->shapes->params == NULL;

	e = ne = 0;
	for( s = d->shapes; s != NULL; s = s->next )
	{
		if( s->params == NULL ) e++; else ne++;
	}

	t = e+ne;

/*
	ManyShapes when:	">1 shape"
	Struct when:		">0 chunks of data to be stored"
	TagField when:		">1 chunks or (>0 chunks && >2 shapes)"
	Union when:		">1 chunks of data to be stored"
	UseNull when:		"firstempty and some data elsewhere"
 */
	d->ManyShapes	= t>1;
	d->Struct	= ne>0;
	d->TagField	= ne>1 || ( ne>0 && t>2);
	d->Union	= ne>1;
	d->UseNull	= ne>0 && firstempty;

	d->PutLoop	= tail_optimize( d );

	if( verbose )
	{
#define XXX(x)	( (x)?' ':'!')
		printf( "type %s: %cManyShapes, %cTagField, ",
			d->name, XXX(d->ManyShapes),
			XXX(d->TagField) );
		printf( "%cStruct, %cUnion, %cUseNull\n",
			XXX(d->Struct), XXX(d->Union),
			XXX(d->UseNull) );
#undef XXX
	}

	if( ! implies(d->Union,d->Struct) )
	{
		fprintf( stderr,
			"optimizer error: type %s has Union&!Struct\n",
			d->name );
		exit(1);
	}
	if( ! implies(d->TagField,d->Struct) )
	{
		fprintf( stderr,
			"optimizer error: type %s has TagField&!Struct\n",
			d->name );
		exit(1);
	}
	if( ! implies(d->UseNull,d->Struct) )
	{
		fprintf( stderr,
			"optimizer error: type %s has UseNull&!Struct\n",
			d->name );
		exit(1);
	}
	if( ! implies(d->TagField,d->ManyShapes) )
	{
		fprintf( stderr,
			"optimizer error: type %s has TagField&!ManyShapes\n",
			d->name );
		exit(1);
	}
	if( ! implies(d->Union,d->TagField) )
	{
		fprintf( stderr,
			"optimizer error: type %s has Union&!TagField\n",
			d->name );
		exit(1);
	}
}


/*
	Given a data decln, check if a tail recursion optimization is possible.
	This is when the last print item of any shape is a number corresponding
	to a field that is the same type as the decln itself.

	Return true if the optimization is possible.
 */

static bool tail_optimize( decln d )
{
	shapelist	s;
	printlist	pl;
	param		p;

	for( s = d->shapes; s != NULL; s = s->next )
	{
		if( s->pl != NULL )
		{
			/* find the last print item */

			for( pl = s->pl; pl->next != NULL; pl = pl->next );

			if( pl->item->tag == printitem_is_num )
			{
				p = findnthparam( pl->item->num, s->params,
						  s->name, d->name );
				if( streq( p->type, d->name ) )
				{
					return true;
				}
			}
		}
	}
	return false;
}
