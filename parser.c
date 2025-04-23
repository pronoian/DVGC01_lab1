/**********************************************************************/
/* lab 1 DVG C01 - Parser OBJECT                                      */
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
 #include "keytoktab.h"         /* when the keytoktab is added   */
/* #include "lexer.h"       */       /* when the lexer     is added   */
/* #include "symtab.h"      */       /* when the symtab    is added   */
/* #include "optab.h"       */       /* when the optab     is added   */

/**********************************************************************/
/* OBJECT ATTRIBUTES FOR THIS OBJECT (C MODULE)                       */
/**********************************************************************/
#define DEBUG 1
static int  lookahead=0;
static int  is_parse_ok=1;

/**********************************************************************/
/* RAPID PROTOTYPING - simulate the token stream & lexer (get_token)  */
/**********************************************************************/
/* define tokens + keywords NB: remove this when keytoktab.h is added */
/**********************************************************************/

/** 
enum tvalues { program=257, id, number, assign, input, output,
               var, begin, end, integer, real, boolean };*/
/**********************************************************************/
/* Simulate the token stream for a given program                      */
/**********************************************************************/
static int tokens[] = {
    program, id, '(', input, ',', output, ')', ';',
    var, id, ',', id, ',', id, ':', integer, ';',
    begin, 
    id, assign, id, '+', id, '*', number,
    end, '.',
};

/**********************************************************************/
/*  Simulate the lexer -- get the next token from the buffer          */
/**********************************************************************/
static int pget_token()
{
    static int i=0;
    if (tokens[i] != '$') return tokens[i++]; else return '$';
}

/**********************************************************************/
/*  PRIVATE METHODS for this OBJECT  (using "static" in C)            */
/**********************************************************************/
static void in(char* s)
{
    if(DEBUG) printf("\n *** In  %s", s);
}
static void out(char* s)
{
    if(DEBUG) printf("\n *** Out %s", s);
}
/**********************************************************************/
/* The Parser functions                                               */
/**********************************************************************/
static void match(int t)
{
    if(DEBUG) printf("\n *** In match \t\t expected %4s found %4s",
                    tok2lex(t), tok2lex(lookahead));
    if (lookahead == t) lookahead = pget_token();
    else {
    is_parse_ok=0;
    printf("\n *** Unexpected Token: expected: %4d found: %4d (in match)",
              t, lookahead);
    }
}

/**********************************************************************/
/* Forward declarations for recursive functions                       */
/**********************************************************************/
static void var_dec_list();
static void stat_list();
static void expr();

/**********************************************************************/
/* The grammar functions                                              */
/**********************************************************************/
static void program_header()
{
    in("program_header");
    match(program); match(id); match('('); match(input);
    match(','); match(output); match(')'); match(';');
    out("program_header");
}

static void type()
{
    in("type");
    if (lookahead == integer) {
        match(integer);
    } else if (lookahead == real) {
        match(real);
    } else if (lookahead == boolean) {
        match(boolean);
    } else {
        is_parse_ok = 0;
        printf("\n *** Syntax: Expected: type found: %d", lookahead);
    }
    out("type");
}

static void id_list()
{
    in("id_list");
    match(id);
    while (lookahead == ',') {
        match(',');
        match(id);
    }
    out("id_list");
}

static void var_dec()
{
    in("var_dec");
    id_list();
    match(':');
    type();
    match(';');
    out("var_dec");
}

static void var_dec_list()
{
    in("var_dec_list");
    var_dec();
    while (lookahead == id) {
        var_dec();
    }
    out("var_dec_list");
}

static void var_part()
{
    in("var_part");
    match(var);
    var_dec_list();
    out("var_part");
}

static void operand()
{
    in("operand");
    if (lookahead == id) {
        match(id);
    } else if (lookahead == number) {
        match(number);
    } else {
        is_parse_ok = 0;
        printf("\n *** Syntax: Expected: operand found: %d", lookahead);
    }
    out("operand");
}

static void factor()
{
    in("factor");
    if (lookahead == '(') {
        match('(');
        expr();
        match(')');
    } else {
        operand();
    }
    out("factor");
}

static void term()
{
    in("term");
    factor();
    while (lookahead == '*') {
        match('*');
        factor();
    }
    out("term");
}

static void expr()
{
    in("expr");
    term();
    while (lookahead == '+') {
        match('+');
        term();
    }
    out("expr");
}

static void assign_stat()
{
    in("assign_stat");
    match(id);
    match(assign);
    expr();
    out("assign_stat");
}

static void stat()
{
    in("stat");
    assign_stat();
    out("stat");
}

static void stat_list()
{
    in("stat_list");
    stat();
    while (lookahead == ';') {
        match(';');
        stat();
    }
    out("stat_list");
}

static void stat_part()
{
    in("stat_part");
    match(begin);
    stat_list();
    match(end);
    match('.');
    out("stat_part");
}

static void prog()
{
    //in("prog");
    program_header();
    var_part();
    stat_part();
    //out("prog");
}

/**********************************************************************/
/*  PUBLIC METHODS for this OBJECT  (EXPORTED)                        */
/**********************************************************************/

int parser()
{
    in("parser");
    lookahead = pget_token();      // get the first token
    prog();                        // call the first grammar rule
    out("parser");
    return is_parse_ok;            // status indicator
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/