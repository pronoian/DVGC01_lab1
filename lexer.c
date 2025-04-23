/**********************************************************************/
/* lab 1 DVG C01 - Lexer OBJECT                                       */
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
#define BUFSIZE 1024
#define LEXSIZE   30
static char buffer[BUFSIZE];
static char lexbuf[LEXSIZE];
static int  pbuf  = 0;               /* current index program buffer  */
static int  plex  = 0;               /* current index lexeme  buffer  */

static void pbuffer();                /* print the program buffer      */
/**********************************************************************/
/*  PRIVATE METHODS for this OBJECT  (using "static" in C)            */
/**********************************************************************/
/**********************************************************************/
/* buffer functions                                                   */
/**********************************************************************/
/**********************************************************************/
/* Read the input file into the buffer                                */
/**********************************************************************/
/**
 * TODO: 
 *  Remove blank spaces in program before reading
 *  Kolla om vissa delar kan implementeras med färdiga funktioner i keytoktab.case
 *  Testa att simplifiera koden med lättare funktioner
 */


static void get_prog()
{
    int c;
    pbuf = 0;                     /* Reset buffer position */
    
    /* Läs in tecken från stdin (där filen är kopplad) tills EOF eller bufferten är full */
    while ((c = getchar()) != EOF && pbuf < BUFSIZE - 1) {
        buffer[pbuf++] = c;      /* Spara tecknet och öka index */
    }
    
    /* Lägg till ett sluttecken i bufferten */
    buffer[pbuf] = '$';          /* $ används som sluttecken */
    
    /* Återställ läspositionen till början av bufferten */
    pbuf = 0;
    pbuffer();
}

/**********************************************************************/
/* Display the buffer                                                 */
/**********************************************************************/

static void pbuffer()
{
    int i;
    printf("\nBuffer contents:\n");
    for (i = 0; buffer[i] != '$' && i < BUFSIZE; i++) {
        putchar(buffer[i]);
    }
    printf("\n");
}

/**********************************************************************/
/* Copy a character from the program buffer to the lexeme buffer      */
/**********************************************************************/

static void get_char()
{
    if (plex < LEXSIZE - 1) {
        /* Kopiera ett tecken från programbufferten till lexembufferten */
        lexbuf[plex++] = buffer[pbuf++];
    }
    else {
        /* Hantera överflöde i lexembufferten */
        printf("\nError: Lexeme buffer overflow\n");
        plex = LEXSIZE - 1;
    }
}

/**********************************************************************/
/* End of buffer handling functions                                   */
/**********************************************************************/

/**********************************************************************/
/*  PUBLIC METHODS for this OBJECT  (EXPORTED)                        */
/**********************************************************************/
/**********************************************************************/
/* Return a token                                                     */
/**********************************************************************/
int get_token()
{
    
    /* Första gången funktionen anropas, läs in programfilen */
    if (pbuf == 0 && buffer[0] != '$') {
        get_prog();
    }
    
    /* Återställ lexembufferten */
    plex = 0;
    memset(lexbuf, 0, LEXSIZE);
    
    /* Hoppa över whitespace (mellanslag, tab, radbrytning) */
    while (buffer[pbuf] == ' ' || buffer[pbuf] == '\t' || buffer[pbuf] == '\n') {
        pbuf++;
    }
    
    /* Kontrollera om vi nått slutet av filen */
    if (buffer[pbuf] == '$') {
        lexbuf[0] = '$';
        lexbuf[1] = '\0';
        return '$';
    }
    
    /* Identifiera identifierare (börjar med en bokstav) */
    if (isalpha(buffer[pbuf])) {
        while (isalpha(buffer[pbuf]) || isdigit(buffer[pbuf])) {
            get_char();
        }
        lexbuf[plex] = '\0';  /* Avsluta strängen */
        return lex2tok(lexbuf);  /* Konvertera till token */
    }
    
    /* Identifiera nummer */
    if (isdigit(buffer[pbuf])) {
        while (isdigit(buffer[pbuf])) {
            get_char();
        }
        lexbuf[plex] = '\0';
        return number;  /* Returnera nummer-token */
    }
    // TODO: Kolla om dessa ⤵️ kan implementeras med keytoktab.c funktioner istället
    /* Hantera speciella operatorer som ':=' */
    if (buffer[pbuf] == ':' && buffer[pbuf+1] == '=') {
        get_char();  /* Läs ':' */
        get_char();  /* Läs '=' */
        lexbuf[plex] = '\0';
        return assign;  /* Returnera tilldelnings-token */
    }
    
    /* Hantera enskilda tecken (operatorer och skiljetecken) */
    get_char();
    lexbuf[plex] = '\0';
    return lex2tok(lexbuf);
}

/**********************************************************************/
/* Return a lexeme                                                    */
/**********************************************************************/
char * get_lexeme()
{
    /* Säkerställ att lexemet är null-terminerat */
    lexbuf[plex] = '\0';
    return lexbuf;
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/
