/*
 *                      DATA DECLARATION BUILDER
 *                      ==== =========== =======
 *
 ******* Author:
 *
 *      Duncan White, Imperial College, London, England.
 *
 ******* Description:
 *
 *      This program builds C data declarations,
 *	constructor and deconstructor functions and write functions
 *	from a series of Haskell/Miranda/Hope style recursive data declarations.
 *	(with optional hints on printing, and optionally, free_* functions)
 *
 *      The output produced is placed in pair of files (eg. x.c and x.h )
 *	which together form a module provided the relevant data types.
 *
 *	NEW in 2018: new "-m" (meta-data only) mode, which causes datadec to
 *	ONLY write a useful summary of types, shapes and the types of the shape
 *	parameters to stdout.
 *	This can be used by experimental client-side tools such as my "C with
 *	shaped pattern matching tool", CPM.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "set.h"
#include "struct.h"
#include "lexer.h"
#include "parser.h"
#include "decs.h"
#include "optimize.h"


#define USAGEMSG "Usage: datadec [-vnof] [-s FNAME [-s OTHERFNAME...]] outfile infile\nOr:   datadec -m infile\n"

#define MUSTBE(b)	{if(!(b)){fprintf(stderr, USAGEMSG);exit(1);}}


#define CHUNKSIZE 10000

typedef char bigstr[ CHUNKSIZE ];

set suppress_funcs;

int main( int argc, char **argv )
{
	char		*basename;
        declnlist	declns;
	int		len;
	bigstr		exports, globals, begin;
	int		gor;

	suppress_funcs = setCreate( NULL );
	verbose = false;
	opt = true;
	metaonly = false;
	while( (gor = getopt(argc, argv, "vfnoms:") ) != -1 )
	{
		switch( gor )
		{
		case 'v':
			verbose = true;
			break;
		case 'f':
			makefree = true;
			break;
		case 'n':
			opt = false;
			break;
		case 'o':
			opt = true;
			break;
		case 'm':
			metaonly = true;
			break;
		case 's':
			setAdd( suppress_funcs, optarg );
			break;
		default:
			MUSTBE(false);
		}
	}

	MUSTBE( optind < argc );

	if( ! metaonly )
	{
		basename = argv[optind++];
		len = strlen( basename );
		if( !strcmp( basename+len-2, ".c" ) )
		{
			basename[len-2] = '\0';
		}
	}

	MUSTBE( optind < argc );

	lexfile = fopen( argv[optind], "r" );
	if( lexfile == NULL )
	{
		fprintf( stderr, "datadec: can't open '%s'\n",
			argv[optind] );
		exit(1);
	}
	optind++;

	MUSTBE( optind == argc );

        if( ! parse_data( exports, globals, begin, &declns ) )
	{
		fprintf( stderr, "datadec: can't parse input properly\n" );
		exit(1);
	}
	if( verbose )
	{
		printf( "datadec: declns are:\n\n" );
		print_declnlist( declns );
		printf( "exports = {%s}\n", exports );
		printf( "globals = {%s}\n", globals );
		printf( "begin = {%s}\n", begin );
		printf( "suppress = " );
		setDump( stdout, suppress_funcs );
		printf( "\n" );
	}
	optimize( declns );
	if( ! metaonly )
	{
		make_declns( exports, globals, begin, declns, suppress_funcs,
			basename );
	} else
	{
		make_metadata( declns );
	}
	setFree( suppress_funcs );
	exit(0);
	/*NOTREACHED*/
}
