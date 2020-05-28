/*
 * longhash.h: hash storage for C, where the values are unsigned longs
 *  a longhash is a hash table of (char *key, unsigned long value) trees..
 *  an useful use for a longhash is as a "bag", aka a "frequency hash".
 *
 * (C) Duncan C. White, 1996-2017 although it seems longer:-)
 */


typedef struct longhash_s *longhash;
typedef unsigned long longhashvalue;
typedef char *longhashkey;

typedef void (*longhashprintfunc)( FILE *, longhashkey, longhashvalue );
typedef void (*longhashforeachcb)( longhashkey, longhashvalue, void * );

extern longhash longhashCreate( longhashprintfunc );
extern void longhashEmpty( longhash );
extern void longhashFree( longhash );
extern void longhashSet( longhash, longhashkey, longhashvalue );
extern long longhashFind( longhash, longhashkey );
extern void longhashInc( longhash, longhashkey );
extern longhash longhashCopy( longhash );
extern void longhashForeach( longhash, longhashforeachcb, void * );
extern void longhashDump( FILE *, longhash );

extern void longhashMetrics( longhash, int *mindepth, int *maxdepth, double *avgdepth );
 /*  calculate the min, max and average depth of all non-empty trees */
