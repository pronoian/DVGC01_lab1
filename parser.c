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
    printf("\n *** Unexpected Token: expected: %4d found: %4d (in match)",
              t, lookahead);
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
        // Get the type of this identifier from symbol table
        result_type = get_ntype(get_lexeme());
        if (result_type == error) {
            printf("\nSemantic: Identifier not declared");
            is_parse_ok = 0;
        }
        match(id);
    } else if (lookahead == number) {
        // Assume all numbers are integers for simplicity
        result_type = integer;
        match(number);
    } else {
        is_parse_ok = 0;
        printf("\n *** Syntax: Expected: operand found: %d", lookahead);
    }
    
    out("operand");
    return result_type;
}

static toktyp factor()
{
    toktyp result_type;
    in("factor");
    
    if (lookahead == '(') {
        match('(');
        result_type = expr();
        match(')');
    } else {
        result_type = operand();
    }
    
    out("factor");
    return result_type;
}

static toktyp term()
{
    toktyp type1, type2, result_type;
    in("term");
    
    type1 = factor();
    result_type = type1;
    
    while (lookahead == '*') {
        match('*');
        type2 = factor();
        // Check operation validity and get result type
        result_type = get_otype('*', type1, type2);
        if (result_type == undef) {
            printf("\nSemantic: Invalid types for * operation: %s and %s", 
                   tok2lex(type1), tok2lex(type2));
            is_parse_ok = 0;
        }
        type1 = result_type; // For chained operations
    }
    
    out("term");
    return result_type;
}

static toktyp expr()
{
    toktyp type1, type2, result_type;
    in("expr");
    
    type1 = term();
    result_type = type1;
    
    while (lookahead == '+') {
        match('+');
        type2 = term();
        // Check operation validity and get result type
        result_type = get_otype('+', type1, type2);
        if (result_type == undef) {
            printf("\nSemantic: Invalid types for + operation: %s and %s", 
                   tok2lex(type1), tok2lex(type2));
            is_parse_ok = 0;
        }
        type1 = result_type; // For chained operations
    }
    
    out("expr");
    return result_type;
}

static void assign_stat()
{
    toktyp id_type, expr_type;
    char *id_name;
    
    in("assign_stat");
    
    // Save the identifier name to look up its type
    id_name = get_lexeme();
    
    // Check if the identifier exists
    if (!find_name(id_name)) {
        printf("\nSemantic: Identifier not declared");
        is_parse_ok = 0;
    }
    
    // Get the type of the identifier
    id_type = get_ntype(id_name);
    
    match(id);
    match(assign);
    
    // Get the type of the expression
    expr_type = expr();
    
    // Check if the assignment is valid
    if (id_type != expr_type) {
        printf("\nSemantic: Assign Types: %s = %s", 
               tok2lex(id_type), tok2lex(expr_type));
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
    prog();                        // call the first grammar rule
    p_symtab();
    out("parser");
    return is_parse_ok;            // status indicator
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/