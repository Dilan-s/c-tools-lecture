#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/*
 *   genfuncs1: read function F,operator Op lines from input,
 *   	        for every such line, produce
 *	         "int F( int a, int b ) { return (a Op b); }"
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
	char line[STRSIZE];
	while( readline(stdin, line, STRSIZE ) != 0 )
	{
		//printf( "// debug: read line '%s'\n", line );
		char *name = strtok( line, "," );
		assert( name != NULL );
		char *op = strtok( NULL, "," );
		assert( op != NULL );
		//printf( "// debug name='%s', op='%s'\n", name, op );
		printf( "int %s( int a, int b ) { return (a%sb); }\n",
			name, op );
	}
	return 0;
}
