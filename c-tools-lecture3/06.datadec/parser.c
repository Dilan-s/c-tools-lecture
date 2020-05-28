/*
 *                           PARSER FOR THE
 *                           ====== === ===
 *
 *                      DATA DECLARATION BUILDER
 *                      ==== =========== =======
 *
 *      This module is the parser used to interprete the high
 *      level data declarations.
 *
 ******* Grammar for Declarations
 *
 *	data   = [ tEXPORT chunk ]
 *		 [ tGLOBAL chunk ]
 *		 [ tBEGIN chunk ]
 *		 tTYPE tOPENCURLY declns tCLOSECURLY
 *	chunk  = tOPENCURLY list_of_lines tCLOSECURLY
 *	declns = list*( decln ) tEOF
 *	decln  = tID tEQ shapes tSEMI
 *	shapes = sep-list+( shape, tOR )
 *	shape  = tID [ tOPENBR params tCLOSEBR ] printlist
 *	params = sep-list+( param, tCOMMA )
 *	param  = [tMINUS] tID tID
 *	printlist = list( tSTR | tNUM )
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "struct.h"
#include "lexer.h"
#include "parser.h"


#define COPYOF(new,old) {new=malloc(1+strlen(old));if(new)strcpy(new,old);}


static bool parse_chunk( char * s );
static bool parse_declns( declnlist * dp );
static void error( char * s );
static bool parse_decln( char ** name, shapelist * shapes );
static bool parse_shapes( shapelist * sp );
static bool parse_shape( char ** tagname, paramlist * pl, printlist * print );
static bool parse_params( paramlist * pp );
static bool parse_printlist( printlist * pp );
static bool parse_printitem( printitem * item );
static bool neverfreetype( char * typename );
static bool parse_param( bool * dontfree, char ** type, char ** name );


#define MUSTBE(t,mesg)	if( nexttok() != (t) ) {error(mesg); return false;}


/*
 *	data   = [ tEXPORT tOPENCURLY crap_to_} ]
 *		 [ tGLOBAL tOPENCURLY crap_to_} ]
 *		 [ tBEGIN tOPENCURLY crap_to_} ]
 *		 tTYPE tOPENCURLY declns tCLOSECURLY
 */

bool parse_data( char *exports, char *globals, char *begin, declnlist *dp )
{
	if( nexttok() == tEXPORT )
	{
		if( ! parse_chunk( exports ) ) return false;
	} else
	{
		*exports = '\0';
		ungettok();
	}

	if( nexttok() == tGLOBAL )
	{
		if( ! parse_chunk( globals ) ) return false;
	} else
	{
		*globals = '\0';
		ungettok();
	}

	if( nexttok() == tBEGIN )
	{
		if( ! parse_chunk( begin ) ) return false;
	} else
	{
		*begin = '\0';
		ungettok();
	}

	MUSTBE( tTYPE, "TYPE/EXPORT/GLOBAL/BEGIN expected" );
	MUSTBE( tOPENCURLY, "{ expected" );
	if( ! parse_declns( dp ) ) return false;
	MUSTBE( tCLOSECURLY, "} expected" );
	MUSTBE( tEOF, "Spurious characters found beyond EOF" );

	return true;
}


/* chunk  = tOPENCURLY list_of_lines tCLOSECURLY */

static bool parse_chunk( char *s )
{
	char line[256];
	int  startlineno;

	*s = '\0';

	MUSTBE( tOPENCURLY, "'{' expected" );

	startlineno = lineno;
	for(;;)
	{
		if( ! readnextline( line ) )
		{
			fprintf( stderr,
				 "Premature EOF after { - started at line %d\n",
				 startlineno );
			return false;
		}
	if( streq( line, "}" ) ) break;
		strcat( s, line );
		strcat( s, "\n" );
	}
	return true;
}


/* declns = list*( decln )       */
/* NB: a decln starts with a tID */

static bool parse_declns( declnlist *dp )
{
	char		*name;
	shapelist	shapes;

	while( nexttok() == tID )
	{
		ungettok();
		if( ! parse_decln( &name, &shapes ) ) return false;
		*dp = build_declnlist( name, shapes, (declnlist) NULL );
		dp = &( (*dp)->next );
	}
	ungettok();
	return true;
}


static void error( char *s )
{
	fprintf( stderr, "%s at line %d\n", s, lineno );
}


/* decln  = tID tEQ shapes tSEMI */

static bool parse_decln( char **name, shapelist *shapes )
{
	MUSTBE( tID, "declaration expected" );
	COPYOF( *name, lexidval );

	MUSTBE( tEQ, "'=' expected" );

	if( ! parse_shapes( shapes ) ) return false;

	MUSTBE( tSEMI, "';' expected" );

	return true;
}


/* shapes = sep-list+( shape, tOR ) */

static bool parse_shapes( shapelist *sp )
{
	char		*tagname;
	paramlist	paras;
	printlist	print;

	for(;;) {
		if( ! parse_shape( &tagname, &paras, &print ) ) return false;
		*sp = build_shapelist( tagname, paras,
				       print, (shapelist) NULL );
	if( nexttok() != tOR ) break;
		sp = &( (*sp)->next );
	}
	ungettok();
	return true;
}


/* shape = tID [ tOPENBR params tCLOSEBR ] printlist */

static bool parse_shape( char **tagname, paramlist *pl, printlist *print )
{
	*pl    = (paramlist) NULL;
	*print = (printlist) NULL;

	MUSTBE( tID, "shape name expected" );
	COPYOF( *tagname, lexidval );

	if( nexttok() == tOPENBR )
	{
		if( ! parse_params( pl ) ) return false;

		MUSTBE( tCLOSEBR, "',' or ')' expected" );
	} else
	{
		ungettok();
	}

	return parse_printlist( print );
}


/* params = sep-list+( param, tCOMMA ) */

static bool parse_params( paramlist *pp )
{
	char *type;
	char *name;
	bool dontfree;

	for(;;) {
		if( !parse_param( &dontfree, &type, &name ) ) return false;
		*pp = build_paramlist( dontfree, type, name, (paramlist) NULL );
	if( nexttok() != tCOMMA ) break;
		pp = &( (*pp)->next );
	}
	ungettok();
	return true;
}


/* printlist = list( printitem ) */
/* NB: Never fails, cos items are one token long */

static bool parse_printlist( printlist *pp )
{
	printitem item;

	*pp = (printlist) NULL;

	while( parse_printitem( &item ) )
	{
		*pp = build_printlist( item, (printlist) NULL );
		pp = &( (*pp)->next );
	}
	return true;
}


/* printitem = tSTR | tNUM */

static bool parse_printitem( printitem *item )
{
	char *temp;

	switch( nexttok() ) {
	case tSTR:
		COPYOF( temp, lexidval );
		*item = build_printitem_str( temp );
		return true;
	case tNUM:
		*item = build_printitem_num( lexintval );
		return true;
	default:
		ungettok();
		return false;
	}
}


/* never free the following types - sorted list */
static char *neverfree[] = {
	"BOOL",
	"bool",
	"char",
	"int",
};

#define NEVERFREE_NEL (sizeof(neverfree)/sizeof(char *))


static bool neverfreetype( char *typename )
{
	int i;
	for( i=0; i<NEVERFREE_NEL; i++ )
	{
		if( strcmp( neverfree[i], typename ) == 0 )
		{
			return true;
		}
	}
	return false;
}


/* param     = [tMINUS] tID tID */

static bool parse_param( bool *dontfree, char **type, char **name )
{
	*dontfree = nexttok() == tMINUS;
	if( ! *dontfree )
	{
		ungettok();
	}
	MUSTBE( tID, "Field type expected" );
	COPYOF( *type, lexidval );

	/* apply standard "never free rules" */
	if( ! *dontfree )
	{
		*dontfree = neverfreetype( *type );
	}

	MUSTBE( tID, "Field name expected" );
	COPYOF( *name, lexidval );
	//printf( "debug: param(%s, %s): dontfree %d\n", *type, *name, *dontfree );

	return true;
}
