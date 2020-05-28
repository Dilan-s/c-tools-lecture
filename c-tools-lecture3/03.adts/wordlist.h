/*
 * simple fixed-size string ("word") list module
 *	all words are stored in each node, strcpy()ied when needed.
 */

#define MAXWORDLEN  128
typedef char wordlistelement[MAXWORDLEN];

typedef struct wordlist *wordlist;
struct wordlist {
	wordlistelement	first;
	wordlist	next;
};


typedef enum {
	wordlist_is_nil,
	wordlist_is_cons,
} kind_of_wordlist;


typedef void (*foreach_wordlist_callback)( char *, void * );


#define wordlist_nil() ((wordlist)NULL)

extern wordlist wordlist_cons( char *first, wordlist next );
extern kind_of_wordlist wordlist_kind( wordlist this );
extern void get_wordlist_cons( wordlist this, wordlistelement first, wordlist * next );
extern void print_wordlist( FILE * f, wordlist p );
extern void sprint_wordlist( char * s, wordlist p );
extern void foreach_wordlist( foreach_wordlist_callback cb, void * data, wordlist p );
extern void free_wordlist( wordlist p );
