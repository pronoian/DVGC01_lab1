/**********************************************************************/
/* lab 1 DVG C01 - Driver OBJECT                                      */
/**********************************************************************/

/**********************************************************************/
/* Include files                                                      */
/**********************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/**********************************************************************/
/* Other OBJECT's METHODS (IMPORTED)                                  */
/**********************************************************************/
#include "keytoktab.h"

/**********************************************************************/
/* OBJECT ATTRIBUTES FOR THIS OBJECT (C MODULE)                       */
/**********************************************************************/
/**********************************************************************/
/* type definitions                                                   */
/**********************************************************************/
typedef struct tab {
	char 	* text;
	int 	token;
}tab;

/**********************************************************************/
/* data objects (tables)                                              */
/**********************************************************************/
static tab tokentab[ ] = {
    {"id", 	            id},
    {"number",      number},
    {":=", 	        assign},
    {"undef", 	     undef},
    {"predef",      predef},
    {"tempty",      tempty},
    {"error",        error},
    {"type",           typ},
    {"$",              '$'},
    {"(",              '('},
    {")",              ')'},
    {"*",              '*'},
    {"+",              '+'},
    {",",              ','},
    {"-",              '-'},
    {".",              '.'},
    {"/",              '/'},
    {":",              ':'},
    {";",              ';'},
    {"=",              '='},
    {"TERROR", 	    nfound}
};


static tab keywordtab[ ] = {
	{"program", 	program},
	{"input", 	      input},
	{"output", 	     output},
	{"var", 	        var},
	{"begin", 	      begin},
	{"end", 	        end},
	{"boolean", 	boolean},
	{"integer", 	integer},
	{"real", 	       real},
	{"KERROR", 	     nfound}
};

/**********************************************************************/
/*  PUBLIC METHODS for this OBJECT  (EXPORTED)                        */
/**********************************************************************/
/**********************************************************************/
/* Display the tables                                                 */
/**********************************************************************/
void p_toktab()
{
    int i;
    
    printf("\n________________________________________________________ ");
    printf("\n THE PROGRAM KEYWORDS ");
    printf("\n________________________________________________________ ");
    
    for (i = 0; keywordtab[i].token != nfound; i++) {
        printf("\n%10s  %3d ", keywordtab[i].text, keywordtab[i].token);
    }
    
    printf("\n________________________________________________________ ");
    printf("\n THE PROGRAM TOKENS ");
    printf("\n________________________________________________________ ");
    
    for (i = 0; tokentab[i].token != nfound; i++) {
        printf("\n%10s  %3d ", tokentab[i].text, tokentab[i].token);
    }
    
    printf("\n________________________________________________________ ");

}

/**********************************************************************/
/* lex2tok - convert a lexeme to a token                              */
/**********************************************************************/
toktyp lex2tok(char * fplex)
{
    int i;
    
    /* Check if it's a keyword */
    toktyp keyword_token = key2tok(fplex);
    if (keyword_token != id) {
        return keyword_token;
    }
    
    /* Check if it's a single character */
    if (strlen(fplex) == 1) {
        char c = fplex[0];
        return (toktyp)c;  /* Return ASCII value directly */
    }
    
    /* Check token table */
    for (i = 0; tokentab[i].token != nfound; i++) {
        if (strcmp(fplex, tokentab[i].text) == 0) {
            return tokentab[i].token;
        }
    }
    
    /* If we get here, assume it's an identifier */
    return id;
}

/**********************************************************************/
/* key2tok - convert a keyword to a token                             */
/**********************************************************************/
toktyp key2tok(char * fplex)
{
    int i;
    
    for (i = 0; keywordtab[i].token != nfound; i++) {
        if (strcmp(fplex, keywordtab[i].text) == 0) {
            return keywordtab[i].token;
        }
    }
    
    /* Not found in keyword table, return nfound */
    return id;
    
    
    /*if (!fplex)
    {
        return nfound;
    }

    for (int i = 0; keywordtab[i].text; i++)
    {
        if (strcmp(fplex, keywordtab[i].text) == 0)
        {
            return keywordtab[i].token;
        }
    }
    
    //printf("\n *** TO BE DONE");  
    return nfound;*/
}

/**********************************************************************/
/* tok2lex - convert a token to a lexeme                              */
/**********************************************************************/
char * tok2lex(toktyp ftok)
{
    int i;
    
    /* Check keyword table for keyword tokens */
    if (ftok >= kstart && ftok < kend) {
        for (i = 0; keywordtab[i].token != nfound; i++) {
            if (keywordtab[i].token == ftok) {
                return keywordtab[i].text;
            }
        }
    }
    
    /* Check token table for non-keyword tokens */
    for (i = 0; tokentab[i].token != nfound; i++) {
        if (tokentab[i].token == ftok) {
            return tokentab[i].text;
        }
    }
    
    /* Not found */
    return "?";
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/
