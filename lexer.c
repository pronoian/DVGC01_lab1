/**  @author: Emil Svensson */ 
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


static void get_prog()
{
    int c;
    pbuf = 0;
    
    // läser in program tills EOF
    while ((c = getchar()) != EOF && pbuf < BUFSIZE - 1) {
        if (c != 13)
        {
            buffer[pbuf++] = c;     
        }
        
    }
    
    // lägg till sluttecken
    buffer[pbuf] = '$';         
    
    pbuf = 0;
    pbuffer();
}

/**********************************************************************/
/* Display the buffer                                                 */
/**********************************************************************/

static void pbuffer()
{
    printf("\n________________________________________________________");
    printf("\n THE PROGRAM TEXT\n");
    printf("________________________________________________________\n");
    for (int i = 0; buffer[i] != '$'; i++) {
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
        return '$';
    }
    
    // Om den börjar med bokstav
    if (isalpha(buffer[pbuf])) {
        while (isalpha(buffer[pbuf]) || isdigit(buffer[pbuf])) {
            get_char();
        }
        return key2tok((char *)&lexbuf);  
    }
    
    // om den är en siffra 
    if (isdigit(buffer[pbuf])) {
        while (isdigit(buffer[pbuf])) {
            get_char();
        }
        return number;  
    }
    // Hatera assign tecken
    if (buffer[pbuf] == ':' && buffer[pbuf+1] == '=') {
        get_char();  // läs :
        get_char();  // läs =
        return assign; 
    }
    
    // enskilda tecken
    get_char();
    return lex2tok(lexbuf);
}

/**********************************************************************/
/* Return a lexeme                                                    */
/**********************************************************************/
char * get_lexeme()
{
    return lexbuf;
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/
