#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/*
 *   genfuncs2: read first line setting Type
 *		followed by "function F,operator Op" lines from input,
 *   	        for every such line, produce
 *	         "Type Type_F( Type a, Type b ) { return (a Op b); }"
 */

int readline( FILE *in, char *buf, int maxlen )
{
	if( fgets( buf, maxlen, in ) == 0 )
	{
		return 0;
	}
	int l = strlen(buf);
	if( l>0 && buf[l-1]=='\n' )
	{
		buf[l-1] = '\0';
	}
	return 1;
}


#define STRSIZE 1024

int main( int argc, char **argv )
{
	char type[STRSIZE];
	assert( readline( stdin, type, STRSIZE ) != 0 );
	//printf( "// debug: type='%s'\n", type );

	char line[STRSIZE];
	while( readline(stdin, line, STRSIZE ) != 0 )
	{
		//printf( "// debug: read line '%s'\n", line );
		char *name = strtok( line, "," );
		assert( name != NULL );
		char *op = strtok( NULL, "," );
		assert( op != NULL );
		//printf( "// debug: name='%s', op='%s'\n", name, op );
		printf( "%s %s_%s( %s a, %s b ) { return (a%sb); }\n",
			type, type, name, type, type, op );
	}
	return 0;
}
