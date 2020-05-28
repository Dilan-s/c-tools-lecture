/*
 * simple string list module
 *	(where a string is a "char *", storage provided outside)
 *
 *	Storage considerations: cons() duplicates the head string,
 *	free_strlist() frees all head strings,
 *	BUT  get_str_cons does not copy the head string,
 *	it simply sets "first" to a ptr-to-the-string inside the list node,
 *	so when traversing you must not free those head pointers.
 */

typedef struct strlist *strlist;
struct strlist {
	char *	first;
	strlist	next;
};


typedef enum {
	strlist_is_nil,
	strlist_is_cons,
} kind_of_strlist;


typedef void (*foreach_strlist_callback)( char *, void * );


#define strlist_nil() ((strlist)NULL)

extern strlist strlist_cons( char *first, strlist next );
extern kind_of_strlist strlist_kind( strlist this );
extern void get_strlist_cons( strlist this, char ** first, strlist * next );
extern void print_strlist( FILE * f, strlist p );
extern void sprint_strlist( char * s, strlist p );
extern void foreach_strlist( foreach_strlist_callback cb, void * data, strlist p );
extern void free_strlist( strlist p );
