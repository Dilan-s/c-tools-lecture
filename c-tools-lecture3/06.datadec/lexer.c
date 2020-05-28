/*
 *                            LEXER FOR THE
 *                            ===== === ===
 *
 *                      DATA DECLARATION BUILDER
 *                      ==== =========== =======
 *
 *      This module is the lexer used by the parser to
 *      interprete the high level data declarations.
 *
 ******* Tokens:
 *
 *      tERROR tEOF tSEMI tID tEQ tOR tOPENBR tCOMMA tCLOSEBR
 *	tSTR tNUM tEXPORT tGLOBAL tBEGIN tOPENCURLY tCLOSECURLY tTYPE
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "struct.h"
#include "lexer.h"


#ifdef DEBUGGING
static char *tokenname[] = {
        "tERROR", "tEOF", "tSEMI", "tID", "tEQ", "tOR", "tOPENBR",
        "tCOMMA", "tCLOSEBR", "tSTR", "tNUM", "tEXPORT", "tGLOBAL",
	"tBEGIN", "tOPENCURLY", "tCLOSECURLY", "tTYPE", "tMINUS",
};
#endif


/* ----------------- Exported variables ---------------- */

#define MAXIDSIZE 100

int  lineno = 1;
char lexidval[ MAXIDSIZE ];
int lexintval;
FILE *lexfile;


/* ----------------- Private variables  ---------------- */

static bool  havepushedtok = false;
static TOKEN curtok;
static char curid[ MAXIDSIZE ];
static int curint;


/* ----------------- Private procedures ---------------- */

static void white_space( void )
{
        int c;

        for(;;) {
                c = getc(lexfile);
		if( c == '/' || c == '#' ) /* comment to end of line */
		{
			while( (c=getc(lexfile)) != EOF && c != '\n' );
		}
                if( c == EOF ) break;
                if( c == '\n' ) lineno++;
                if( c != ' ' && c != '\t' && c != '\n' ) break;
        }
        ungetc( c, lexfile );
}


/* ----------------- Public procedures  ---------------- */

void ungettok( void )
{
        if( havepushedtok )
	{
		fprintf( stderr, "ungettok: can't push 2 tokens\n" );
		exit(1);
	}
        havepushedtok = true;
#ifdef DEBUGGING
printf( "lexer: ungot token %s\n", tokenname[ curtok ] );
#endif
}


TOKEN nexttok( void )
{
	int c;
	int  pos;

        if( havepushedtok )
	{
                havepushedtok = false;
        } else
	{
                white_space();
                c = getc(lexfile);
		switch( c )
		{
                case EOF: curtok = tEOF; break;
		case ';': curtok = tSEMI; break;
		case '{': curtok = tOPENCURLY; break;
		case '}': curtok = tCLOSECURLY; break;
                case '(': curtok = tOPENBR; break;
                case ')': curtok = tCLOSEBR; break;
                case ',': curtok = tCOMMA; break;
                case '|': curtok = tOR; break;
                case '-': curtok = tMINUS; break;
                case '=': curtok = tEQ; break;
		case '"':
			for( pos=0; (c=getc(lexfile)) != '\"'; )
			{
				if( pos<MAXIDSIZE-1 )
				{
					lexidval[pos++] = c;
				}
			}
			while( pos<MAXIDSIZE )
			{
				lexidval[pos++] = '\0';
			}
			strcpy( curid, lexidval );
			curtok = tSTR;
			break;
		default:
			if( isalpha( c ) )
			{
				curtok = tID;
				for( pos=0; isalpha(c)||isdigit(c)||c=='_'; )
				{
					if( pos<MAXIDSIZE-1 )
						lexidval[pos++] = c;
					c=getc(lexfile);
				}
				ungetc( c, lexfile );
				while( pos<MAXIDSIZE )
					lexidval[pos++] = '\0';
				strcpy( curid, lexidval );
				if( streq( curid, "EXPORT" ) )
				{
					curtok = tEXPORT;
				} else if( streq( curid, "GLOBAL" ) )
				{
					curtok = tGLOBAL;
				} else if( streq( curid, "BEGIN" ) )
				{
					curtok = tBEGIN;
				} else if( streq( curid, "TYPE" ) )
				{
					curtok = tTYPE;
				} else if( streq( curid, "OR" ) )
				{
					curtok = tOR;
				} else if( streq( curid, "or" ) )
				{
					curtok = tOR;
				}
			} else if( isdigit( c ) )
			{
				int t;

				curtok = tNUM;
				for( t=0; isdigit(c); )
				{
					t = t*10 + c-'0';
					c=getc(lexfile);
				}
				ungetc( c, lexfile );
				curint = lexintval = t;
			} else
			{
				curtok = tERROR;
			}
		}
        }
#ifdef DEBUGGING
printf( "returning token %s\n", tokenname[ curtok ] );
#endif
        return curtok;
}


bool readnextline( char *line )
{
	int c;
	char *s = line;

	while( (c=getc(lexfile)) != EOF && c != '\n' ) *s++ = c;
	*s = '\0';
	lineno++;
	return c != EOF;
}
