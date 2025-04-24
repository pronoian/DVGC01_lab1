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
 * Ta bort kommentarer
 */


static void get_prog()
{
    int c;
    pbuf = 0;                     /* Reset buffer position */
    
    /* Läs in tecken från stdin (där filen är kopplad) tills EOF eller bufferten är full */
    while ((c = getchar()) != EOF && pbuf < BUFSIZE - 1) {
        if (c != 13)
        {
            buffer[pbuf++] = c;      /* Spara tecknet och öka index */
        }
        
    }
    
    /* Lägg till ett sluttecken i bufferten */
    buffer[pbuf] = '$';         
    
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
    printf("\n________________________________________________________");
    printf("\n THE PROGRAM TEXT:\n");
    printf("________________________________________________________\n");
    for (i = 0; buffer[i] != '$' && i < BUFSIZE; i++) {
        printf("%c", (buffer[i]));
    }
    printf("$\n");

    printf("________________________________________________________");
    
}

/**********************************************************************/
/* Copy a character from the program buffer to the lexeme buffer      */
/**********************************************************************/

static void get_char()
{
    if (plex < LEXSIZE - 1) {
        // kopiera till lexem buffert
        lexbuf[plex++] = buffer[pbuf++];
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
   
    if (pbuf == 0 && buffer[0] != '$') 
        get_prog();
    
    plex = 0;
    memset(lexbuf, 0, LEXSIZE);
    while (buffer[pbuf] == ' ' || buffer[pbuf] == '\t' || buffer[pbuf] == '\n') {
        pbuf++;
    }
    // EOF
    if (buffer[pbuf] == '$') {
        lexbuf[0] = '$';
        //lexbuf[1] = '\0';
        return '$';
    }
    
    // Om den börjar med bokstav är det ID 
    if (isalpha(buffer[pbuf])) {
        while (isalpha(buffer[pbuf]) || isdigit(buffer[pbuf])) {
            get_char();
        }
        //lexbuf[plex] = '\0';  
        return key2tok((char *)&lexbuf);  
    }
    
    // om den är en siffra 
    if (isdigit(buffer[pbuf])) {
        while (isdigit(buffer[pbuf])) {
            get_char();
        }
        //lexbuf[plex] = '\0';
        return number;  
    }
    // Hatera assign opperator
    if (buffer[pbuf] == ':' && buffer[pbuf+1] == '=') {
        get_char();  // läs :
        get_char();  // läs =
        //lexbuf[plex] = '\0';
        return assign; 
    }
    
    // enskilda tecken
    get_char();
    //lexbuf[plex] = '\0';
    return lex2tok(lexbuf);
}

/**********************************************************************/
/* Return a lexeme                                                    */
/**********************************************************************/
char * get_lexeme()
{
    //lexbuf[plex] = '\0';
    return lexbuf;
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/
