/*
 * longset.h: set of longs (based on hash) storage for C..
 *
 * (C) Duncan C. White, 1996-2019 although it seems longer:-)
 */

/*
 * a longset is a hash table of key trees..
 */

typedef struct longset_s *longset;

typedef void (*longsetprintfunc)( FILE *, long );
typedef void (*longsetforeachcb)( long, void * );

extern longset longsetCreate( longsetprintfunc p );
extern void longsetEmpty( longset s );
extern longset longsetCopy( longset s );
extern void longsetFree( longset s );
extern void longsetMetrics( longset s, int * min, int * max, double * avg );
extern void longsetAdd( longset s, long k );
extern void longsetRemove( longset s, long k );
extern void longsetModify( longset s, char * changes );
extern bool longsetIn( longset s, long k );
extern void longsetForeach( longset s, longsetforeachcb cb, void * arg );
extern void longsetUnion( longset a, longset b );
extern void longsetIntersection( longset a, longset b );
extern void longsetDiff( longset a, longset b );
extern int longsetNMembers( longset s );
extern bool longsetIsEmpty( longset s );
extern void longsetSubtraction( longset a, longset b );
extern void longsetDump( FILE * out, longset s );
