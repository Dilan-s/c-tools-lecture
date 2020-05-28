/*
 *   readline.c: utility function readline..
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "readline.h"


// bool ok = readline( fh, buff, maxlen );
//	try to read a complete line from <fh> into <buff>,
//	but read no more than <maxlen> characters.
//	remove the trailing '\n' if there is one.
//	return false if EOF, else true if there's a line.
bool readline( FILE *in, char *buf, int maxlen )
{
	if( fgets( buf, maxlen, in ) == 0 )
	{
		return false;
	}
	int l = strlen(buf);
	if( l>0 && buf[l-1]=='\n' )
	{
		buf[l-1] = '\0';
	}
	return true;
}
