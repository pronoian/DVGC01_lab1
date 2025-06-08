/**  @author: Emil Svensson */ 
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
 #include "lexer.h"              /* when the lexer     is added   */
 #include "symtab.h"             /* when the symtab    is added   */
 #include "optab.h"              /* when the optab     is added   */

/**********************************************************************/
/* OBJECT ATTRIBUTES FOR THIS OBJECT (C MODULE)                       */
/**********************************************************************/
#define DEBUG 0
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
/*static int tokens[] = {
    program, id, '(', input, ',', output, ')', ';',
    var, id, ',', id, ',', id, ':', integer, ';',
    begin, 
    id, assign, id, '+', id, '*', number,
    end, '.',
};*/

/**********************************************************************/
/*  Simulate the lexer -- get the next token from the buffer          */
/**********************************************************************/
/*static int pget_token()
{
    static int i=0;
    if (tokens[i] != '$') return tokens[i++]; else return '$';
}*/

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
    if (lookahead == t) lookahead = get_token();
    else {
        is_parse_ok=0;
        printf("\nSYNTAX: Symbol expected %s found %s",
                    tok2lex(t), tok2lex(lookahead));
    }
}

/**********************************************************************/
/* Forward declarations for recursive functions                       */
/**********************************************************************/
static void var_dec_list();
static void stat_list();
static toktyp expr();

/**********************************************************************/
/* The grammar functions                                              */
/**********************************************************************/
static void program_header()
{
    p_symtab();
    in("program_header");
    match(program); 
    addp_name(get_lexeme());
    match(id);
    match('('); match(input);
    match(','); match(output); match(')'); match(';');
    out("program_header");
}

static void type()
{
    in("type");
    if (lookahead == integer) {
        setv_type(integer);
        match(integer);
    } else if (lookahead == real) {
        setv_type(real);
        match(real);
    } else if (lookahead == boolean) {
        setv_type(boolean);
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
    addv_name(get_lexeme());
    match(id);
    while (lookahead == ',') { 
        match(',');
        addv_name(get_lexeme());
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

static toktyp operand()
{
    toktyp result_type = undef;
    in("operand");
    
    if (lookahead == id) {
        result_type = get_ntype(get_lexeme());
        if (result_type == error) {
            printf("\nSEMANTIC: ID NOT declared: %s", (get_lexeme())); // ...fixed... kommentar
            is_parse_ok = 0;
        }
        match(id);
    } else if (lookahead == number) {
        // Assume all numbers are integers for now
        result_type = integer;
        match(number);
    } else {
        is_parse_ok = 0;
        printf("\nSYNTAX:\tExpected: operand found: %s", tok2lex(lookahead));
    }
    
    out("operand");
    return result_type;
}

static toktyp factor()
{
    in("factor");
    toktyp res; 
    
    if (lookahead == '(') {
        match('(');
        res = expr();
        match(')');
    } else {
        res = operand();
    }
    
    out("factor");
    return res;
}

static toktyp term()
{
    in("term");
    toktyp left = factor();
    toktyp right; 
    toktyp result = left;  
    
    while (lookahead == '*') {
        match('*');
        right = factor();
        
        result = get_otype('*', left, right);
        left = result;
    }
    
    out("term");
    return result;  // Return final type
}

static toktyp expr()
{
    in("expr");
    toktyp t1 = term();
    toktyp result = t1;
    
    // Handle + operations
    while (lookahead == '+') {
        match('+');
        toktyp t2 = term();
        // Get result type
        toktyp temp = get_otype('+', t1, t2);
        if (temp == undef) { 
            is_parse_ok = 0;
        }
        
        result = temp;
        t1 = result;  
    }
    out("expr");
    return result;
}

static void assign_stat() // ...fixed... skrev om assign stat lite
{
    in("assign_stat");
    
    // Get var name
    char* var_name = get_lexeme();
    toktyp left_type = get_ntype(var_name);
    
    if (left_type == error) {
        printf("\nSEMANTIC: ID NOT declared: %s", var_name);
        is_parse_ok = 0;
        left_type = undef;  // Använd undef för jämförelsen
    }
    
    match(id);
    match(assign);
    
    toktyp right_type = expr();
    
    // Jämför bara om vänster sida inte är undef
    if (left_type != undef && right_type != undef && left_type != right_type) {
        printf("\nSEMANTIC: Assign types: %s := %s", 
               tok2lex(left_type), tok2lex(right_type));
        is_parse_ok = 0;
    }
    
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
    lookahead = get_token();      // get the first token
    prog();
    p_symtab();
    out("parser");
    return is_parse_ok;
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/