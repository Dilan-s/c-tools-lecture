/*
 * simple fixed-string ("word") list module: the implementation
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "wordlist.h"


#define NEW(t) ((t)malloc(sizeof(struct t)))


wordlist wordlist_cons( char *first, wordlist next )
{
	assert( strlen(first) < MAXWORDLEN );
	wordlist new = NEW(wordlist);
	strcpy( new->first, first );
	new->next   = next;
	return new;
}


kind_of_wordlist wordlist_kind( wordlist this )
{
	if( this == NULL )
	{
		return wordlist_is_nil;
	}
	return wordlist_is_cons;
}


void get_wordlist_cons( wordlist this, wordlistelement first, wordlist *next )
{
	strcpy( first, this->first );
	*next  = this->next;
}


void print_wordlist( FILE *f, wordlist p )
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


void sprint_wordlist( char *s, wordlist p )
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


void foreach_wordlist( foreach_wordlist_callback cb, void *data, wordlist p )
{
	while( p != NULL )
	{
		(*cb)( p->first, data );
		p = p->next;
	}
}


void free_wordlist( wordlist p )
{
	while( p != NULL )
	{
		wordlist pn = p->next;
		free( (void *) p );
		p = pn;
	}
}
