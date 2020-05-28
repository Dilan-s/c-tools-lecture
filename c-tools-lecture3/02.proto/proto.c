/*
 ****** ANSI C Prototypes builder...
 *	============================
 *
 *		Copyright (C) Duncan White, 1987-1988.
 *		Major rewrite in Jun 1998
 ****** Purpose
 *
 *	This program superficially parses a C file, and extracts function
 *	prototypes for all the (static/public) functions declared in the given
 *	C file..
 *	You can then incorporate the file generated into your .h files.
 *
 *	Alternatively, if you give the -f [function] option, the program will
 *	instead copy the entire C file, replacing all the function headings
 *	with the ANSI C form..
 *
 ****** How does it do it:
 *
 *	The program parses EACH LINE of a C source file to see whether it
 *	satisfies either the K&R or ANSI grammars shown below. If it does, the
 *	appropriate declaration is bunged out onto stdout.
 *
 *	Unfortunately, this implies that an entire function declaration
 *	must be on a single line.... sorry to all those who don't use this
 *	convention: I didn't feel up to writing a full C parser, or a
 *	backtracking parser!
 *
 ****** The Grammar
 *
 *	The grammar is restricted; in particular, it omits:
 *
 *	1).	function parameters (use typedef!),
 *	2).	array parameters (I use pointers instead),
 *
 *	k&r_function_dec	= [static] [struct|union] id starred_id '(' idlist ')' tslist
 *	idlist			= empty | id list{ ',' id }
 *	tslist			= list{ typespec } ( EOL | '{' )
 *	typespec		= [struct|union] id starred_id list{ ',' starred_id } ';'
 *	starred_id		= list{'*'} id
 *
 *	ansi_function_dec	= [ starred_id ] id '(' ansiparamlist ')'
 *	ansiparamlist		= empty | void | id starred_id
 *					list{ ',' id starred_id }
 */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define NEW(t)     ( (t) malloc(sizeof(struct t)) )

typedef char BOOL;
#define TRUE   1
#define FALSE  0

#undef HASPROTOS



/* ----------------------- Data types ------------------------- */


#define IDLEN		50
#define LINELEN		1024

typedef char   ident[IDLEN];
typedef char   wholeline[LINELEN];


typedef struct IDLIST *IDLIST;
struct IDLIST
{
	ident	id;
	IDLIST	next;
};

typedef struct TSLIST *TSLIST;
struct TSLIST
{
	ident	type;
	ident	name;
	TSLIST	next;
};

typedef struct
{
	ident	type;
	ident	name;
	IDLIST	formals;
	TSLIST	ftypes;
	BOOL	isstatic;
} FDECL;


/* --------------------- Forward declns ----------------------- */
/*             Guess what generated these then ???              */


static void shift( char **, int * );
static void usage( BOOL );
static void print_fdecl( FILE *, FDECL *, BOOL );
static void print_param( FILE *, ident, FDECL * );
static int readline( FILE *, char * );
static void get_token( void );
static BOOL parse_kr_func( char *, FDECL * );
static BOOL parse_ansi_func( char *, FDECL * );
static BOOL parse_list_id( IDLIST * );
static BOOL parse_restof_ids( IDLIST * );
static BOOL parse_ansi_paramlist( IDLIST *, TSLIST * );
static BOOL parse_restof_ansi_params( IDLIST *, TSLIST * );
static BOOL parse_list_ts( TSLIST * );
static BOOL parse_typespec( TSLIST * );
static BOOL parse_starred_id( char *, TSLIST );


/* ---------------------- Main program ------------------------ */


int main( int argc, char ** argv )
{
	FILE		*in, *out;
	FDECL		f;
	wholeline	line;
	BOOL		reformat;
	BOOL		showstatic;

	usage( argc > 1 && argc <= 4 );

	showstatic = ( strcmp( argv[1], "-s" ) == 0 );
	reformat = ( strcmp( argv[1], "-f" ) == 0 );
	if( showstatic || reformat )
	{
		shift( argv, &argc );
	}

	usage( argc > 1 );	/* still at least one argument left */

	char *filename = strdup( argv[1] );
	int len = strlen(filename);
	/* If we're given the name of a .h file, use the .c instead */
	if( filename[len-1] == 'h' )
	{
		 filename[len-1] = 'c';
	}

	in = fopen( filename, "r" );
	if( in == NULL )
	{
		fprintf( stderr, "Can't open '%s'\n", filename );
		exit(1);
	}
	free( filename );

	if( argc == 2 )
	{
		out = stdout;
	} else
	{
		out = fopen( argv[2], "w" );
		if( out == NULL )
		{
			fprintf( stderr, "Can't create '%s'\n", argv[2] );
			exit(1);
		}
	}

	while( readline( in, line ) != EOF )
	{

		if( parse_ansi_func( line, &f ) ||
		    parse_kr_func( line, &f ) )
		{
			if( reformat || f.isstatic == showstatic )
			{
				print_fdecl( out, &f, ! reformat );
			}
		} else
		{
			if( reformat )
			{
				fprintf( out, "%s\n", line );
			}
		}
	}
	fclose( in );
	fclose( out );
	exit( 0 );
	return( 0 );
	/*NOTREACHED*/
}


static void shift( char ** argv, int * argc )
{
	int i;

	for( i = 2; i < *argc; i++ ) argv[i-1] = argv[i];
	(*argc)--;
}


static void usage( BOOL b )
{
	if( ! b )
	{
		fprintf( stderr, "Usage: proto [-s|-f] infile [outfile]\n" );
		exit(1);
	}
}


/* print the function declaration or prototype, now in ANSI C */

static void print_fdecl( FILE * out, FDECL * f, BOOL prototype )
{
	if( f->isstatic )
	{
		fprintf( out, "static " );
	} else if( prototype )
	{
		fprintf( out, "extern " );
	}
	fprintf( out, "%s %s( ", f->type, f->name );
	if( f->formals )
	{
		IDLIST i;
		BOOL comma = FALSE;

		for( i = f->formals; i; i = i->next )
		{
			if( comma )
			{
				fprintf( out, ", " );
			}
			comma = TRUE;
			print_param( out, i->id, f );
		}
	} else
	{
		fprintf( out, "void" );
	}
	fprintf( out, prototype ? " );\n" : " )\n" );
}


static void print_param( FILE * out, ident name, FDECL * f )
{
	TSLIST t;

	for( t = f->ftypes; t; t=t->next )
	{
		if( strcmp( name, t->name ) == 0 )
		{
			fprintf( out, "%s %s", t->type, t->name );
			return;
		}
	}
	fprintf( stderr, "proto: %s has undeclared formal '%s'\n",
		f->name, name );
	exit(1);
}


static int readline( FILE * f, char * line )
{
	int c;
	char *p = line;

	c=getc(f);
	if( c==EOF ) return c;
	ungetc( c, f );
	while( (c=getc(f)) != '\n' ) *p++ = c;
	*p = '\0';
	return p-line;
}


/* ----------------- Lexer routines ---------------------- */


char *lex_line;
char tokstr[100];
enum token_type
{
	tID, tSTAR, tSEMI, tCOMMA, tOPEN, tCLOSE, tEOL, tBRACE, tSTRUCT,
	tUNION, tSTATIC, tERR
} token;


static void get_token( void )
{
	char *s = tokstr;
	char c;

	while( *lex_line == ' ' || *lex_line == '\t' ) lex_line++;
	c = *lex_line++;
	*s++ = c;
	*s = '\0';

	switch( c )
	{
	case '\0' : token = tEOL      ; break;
	case ','  : token = tCOMMA    ; break;
	case ';'  : token = tSEMI     ; break;
	case '*'  : token = tSTAR     ; break;
	case '('  : token = tOPEN     ; break;
	case '{'  : token = tBRACE; break;
	case ')'  : token = tCLOSE    ; break;
	default   :
		if( !isalpha( c ) )
		{
			token = tERR;
		} else
		{
			while( (c = *lex_line)=='_'||isalnum(c) )
			{
				*s++ = c; lex_line++;
			}
			*s = '\0';
			if( strcmp(tokstr,"struct") == 0 )
			{
				token = tSTRUCT;
			} else if( strcmp(tokstr,"union") == 0 )
			{
				token = tUNION;
			} else if( strcmp(tokstr,"static") == 0 )
			{
				token = tSTATIC;
			} else if( strcmp(tokstr,"if") == 0
			||  strcmp(tokstr,"while") == 0
			||  strcmp(tokstr,"switch") == 0
			)
			{
				token = tERR;
			} else
			{
				token = tID;
			}
		}
	}
}


/* ------------------- Parser routines ------------------- */


static BOOL parse_kr_func( char * line, FDECL * f )
{
	char basetype[IDLEN];
	char prefix  [IDLEN];

	lex_line = line;
	get_token();
	strcpy( prefix, "" );
	f->isstatic = token == tSTATIC;
	if( f->isstatic )
	{
		get_token();
	}
	if( token == tSTRUCT || token == tUNION )
	{
		strcpy( prefix, tokstr );
		get_token();
	}

	if( token != tID ) return FALSE;

	if( *prefix != '\0' )
	{
		strcpy( basetype, prefix );
		strcat( basetype, " " );
		strcat( basetype, tokstr );
	} else
	{
		strcpy( basetype, tokstr );
	}
	get_token();
	if( token == tOPEN )
	{
		strcpy( f->name, basetype );
		strcpy( f->type, "int" );
	} else
	{
		TSLIST ts = NEW(TSLIST);

		if( ! parse_starred_id( basetype, ts ) ) return FALSE;
		strcpy( f->type, ts->type );
		strcpy( f->name, ts->name );

		if( token != tOPEN ) return FALSE;
	}
	get_token();
	if( ! parse_list_id( &(f->formals) ) || token != tCLOSE )
	{
		return FALSE;
	}
	get_token();
	return parse_list_ts( &(f->ftypes) );
}


static BOOL parse_ansi_func( char * line, FDECL * f )
{
	char basetype[IDLEN];
	char prefix  [IDLEN];

	lex_line = line;
	get_token();
	strcpy( prefix, "" );
	f->isstatic = token == tSTATIC;
	if( f->isstatic )
	{
		get_token();
	}
	if( token == tSTRUCT || token == tUNION )
	{
		strcpy( prefix, tokstr );
		get_token();
	}

	if( token != tID ) return FALSE;

	if( *prefix != '\0' )
	{
		strcpy( basetype, prefix );
		strcat( basetype, " " );
		strcat( basetype, tokstr );
	} else
	{
		strcpy( basetype, tokstr );
	}

	/*printf( "parse_ansi_func: found line %s\n", line ); */

	strcpy( basetype, tokstr );
	get_token();
	if( token == tOPEN )
	{
		strcpy( f->type, "int" );
		strcpy( f->name, basetype );
	} else
	{
		TSLIST ts = NEW(TSLIST);

		if( ! parse_starred_id( basetype, ts ) ) return FALSE;
		strcpy( f->type, ts->type );
		strcpy( f->name, ts->name );

		if( token != tOPEN ) return FALSE;
	}
	get_token();
	if( ! parse_ansi_paramlist( &(f->formals), &(f->ftypes) )
	||  token != tCLOSE )
	{
		return FALSE;
	}
	get_token();
	return ( token==tEOL || token==tBRACE );
}


/* attempt to parse a K&R idlist [possibly empty] */

static BOOL parse_list_id( IDLIST * ip )
{
	*ip = NULL;
	if( token != tID ) return TRUE;

	*ip = NEW( IDLIST );
	strcpy( (*ip)->id, tokstr );
	get_token();

	if( token != tCOMMA && token != tCLOSE ) return FALSE;

	return parse_restof_ids( &((*ip)->next) );
}


static BOOL parse_restof_ids( IDLIST * ip )
{
	*ip = NULL;
	if( token != tCOMMA ) return TRUE;

	get_token();
	if( token != tID ) return FALSE;

	*ip = NEW( IDLIST );
	strcpy( (*ip)->id, tokstr );
	get_token();
	return parse_restof_ids( &((*ip)->next) );
}


/* attempt to parse an ANSI C param list */
static BOOL parse_ansi_paramlist( IDLIST * ip, TSLIST * tp )
{
	char basetype[IDLEN];

	*ip = NULL;
	*tp = NULL;

	/* could be () which isn't strict ANSI but ok... */
	if( token != tID )
	{
		/*printf( "found empty paramlist..\n" ); */
		return TRUE;
	}

	strcpy( basetype, tokstr );
	get_token();

	/* could be an ANSI C "( void )" procedure.. */
	if( token == tCLOSE && strcmp( basetype, "void" ) == 0 )
	{
		/* printf( "found void paramlist..\n" ); */
		return TRUE;
	}

	*tp = NEW( TSLIST );

	if( ! parse_starred_id( basetype, *tp ) )
	{
		*tp = NULL;
		return FALSE;
	}
	/* printf( "found starred id .. basetype = %s, type = %s, name = %s\n",
		basetype, (*tp)->type, (*tp)->name );
	*/

	*ip = NEW( IDLIST );
	strcpy( (*ip)->id, (*tp)->name );

	return parse_restof_ansi_params( &((*ip)->next), &((*tp)->next) );
}


/* attempt to parse rest of an ANSI C param list */
/* list{ , id stars id } */

static BOOL parse_restof_ansi_params( IDLIST * ip, TSLIST * tp )
{
	char basetype[IDLEN];

	*ip = NULL;
	*tp = NULL;

	while( token == tCOMMA )
	{
		/* printf( "found comma in paramlist\n" ); */

		get_token();
		if( token != tID )
		{
			return FALSE;
		}
		/* printf( "found id %s in paramlist\n", tokstr ); */

		strcpy( basetype, tokstr );

		*tp = NEW( TSLIST );

		get_token();
		if( ! parse_starred_id( basetype, *tp ) ) return FALSE;

		/* printf( "found starred id .. type = %s, name = %s\n",
			(*tp)->type, (*tp)->name );
		*/

		*ip = NEW( IDLIST );
		strcpy( (*ip)->id, (*tp)->name );

		ip = &((*ip)->next);
		tp = &((*tp)->next);
	}
	/* printf( "found complete paramlist.. returning true\n" ); */
	return TRUE;
}


/*
 * attempt to parse a list of typespecs [possibly empty]
 * followed by tEOL or tBRACE
 */

static BOOL parse_list_ts( TSLIST * tp )
{
	TSLIST t;

	for(;;)
	{
		*tp = NULL;
		if( token==tEOL || token==tBRACE ) return TRUE;

		if( ! parse_typespec( tp ) ) return FALSE;
		for( t = *tp; t->next; t=t->next );
		tp = &(t->next);
	}
	/*NOTREACHED*/
}


static BOOL parse_typespec( TSLIST * tp )
{
	char basetype[IDLEN];

	if( token != tID ) return FALSE;
	strcpy( basetype, tokstr );

	for(;;)
	{
		get_token();
		*tp = NEW( TSLIST );
		if( ! parse_starred_id( basetype, *tp ) ) return FALSE;

		if( token == tSEMI )
		{
			get_token();
			return TRUE;
		}
		if( token != tCOMMA ) return FALSE;
		tp = &((*tp)->next);
	}
	/*NOTREACHED*/
}


static BOOL parse_starred_id( char * basetype, TSLIST tp )
{
	BOOL ok;
	char thestars[IDLEN];
	char *stars = thestars;

	while( token == tSTAR )
	{
		*stars++ = '*';
		get_token();
	}
	*stars = '\0';
	ok = token == tID;
	if( ok )
	{
		if( *thestars == '\0' )
		{
			strcpy( tp->type, basetype );
		} else
		{
			sprintf( tp->type, "%s %s", basetype, thestars );
		}

		strcpy( tp->name, tokstr );
		get_token();
	}
	return ok;
}
