/*
 *                        LEXER FOR THE
 *                        ===== === ===
 *
 *                  DATA DECLARATION BUILDER
 *                  ==== =========== =======
 *
 *      This module is the lexer used by the parser to
 *      interprete the high level data declarations.
 */


/* ----------------- Exported defns     ---------------- */

typedef enum {
        tERROR, tEOF, tSEMI, tID, tEQ, tOR,
        tOPENBR, tCOMMA, tCLOSEBR, tSTR, tNUM,
	tEXPORT, tGLOBAL, tBEGIN, tOPENCURLY,
	tCLOSECURLY, tTYPE, tMINUS,
} TOKEN;



/* ----------------- Exported variables ---------------- */

extern char lexidval[];
extern int  lexintval;
extern int  lineno;
extern FILE *lexfile;


/* ----------------- Public procedures  ---------------- */

extern void ungettok( void );
extern TOKEN nexttok( void );
extern bool readnextline( char * line );
