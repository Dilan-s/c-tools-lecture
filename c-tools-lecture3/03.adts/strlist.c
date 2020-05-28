/*
 * simple string list module: the implementation
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "strlist.h"


#define NEW(t) ((t)malloc(sizeof(struct t)))


strlist strlist_cons( char *first, strlist next )
{
	strlist	new = NEW(strlist);
	new->first  = strdup(first);
	new->next   = next;
	return new;
}


kind_of_strlist strlist_kind( strlist this )
{
	if( this == NULL )
	{
		return strlist_is_nil;
	}
	return strlist_is_cons;
}


void get_strlist_cons( strlist this, char **first, strlist *next )
{
	*first = this->first;
	*next  = this->next;
}


void print_strlist( FILE *f, strlist p )
{
	fputs( "[ ", f );
	while( p != NULL )
	{
		fprintf( f, "%s", p->first );
		if( p->next != NULL )
		{
			fputs( ",", f );
		}
		p = p->next;
	}
	fputs( " ]", f );
}


void sprint_strlist( char *s, strlist p )
{
	strcpy( s, "[ " );
	while( p != NULL )
	{
		strcat( s, p->first );
		if( p->next != NULL )
		{
			strcat( s, "," );
		}
		p = p->next;
	}
	strcat( s, " ]" );
}


void foreach_strlist( foreach_strlist_callback cb, void *data, strlist p )
{
	while( p != NULL )
	{
		(*cb)( p->first, data );
		p = p->next;
	}
}


void free_strlist( strlist p )
{
	while( p != NULL )
	{
		strlist pn = p->next;
		free( (void *) p->first );
		free( (void *) p );
		p = pn;
	}
}
