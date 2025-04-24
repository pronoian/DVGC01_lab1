/**********************************************************************/
/* lab 1 DVG C01 - Symbol Table OBJECT                                */
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
#define TABSIZE 1024                   /* symbol table size           */
#define NAMELEN   20                   /* name length                 */

typedef char tname[NAMELEN];

static tname  name[TABSIZE];
static toktyp role[TABSIZE];
static toktyp type[TABSIZE];
static int    size[TABSIZE];
static int    addr[TABSIZE];

static int numrows=0;                  /* number of rows in the ST    */
static int startp =0;                  /* start position program in ST*/

/**********************************************************************/
/*  PRIVATE METHODS for this OBJECT  (using "static" in C)            */
/**********************************************************************/
/**********************************************************************/
/*  GET methods (one for each attribute)                              */
/**********************************************************************/
static char * get_name(int ftref)   { return name[ftref]; }
static toktyp get_role(int ftref)   { return role[ftref]; }
static toktyp get_type(int ftref)   { return type[ftref]; }
static int    get_size(int ftref)   { return size[ftref]; }
static int    get_addr(int ftref)   { return addr[ftref]; }

/**********************************************************************/
/*  SET methods (one for each attribute)                              */
/**********************************************************************/
static void set_name(int ftref, char * fpname) { strcpy(name[ftref],
                                                        fpname);}
static void set_role(int ftref, toktyp frole)  { role[ftref] = frole; }
static void set_type(int ftref, toktyp ftype)  { type[ftref] = ftype; }
static void set_size(int ftref, int    fsize)  { size[ftref] = fsize; }
static void set_addr(int ftref, int    faddr)  { addr[ftref] = faddr; }

/**********************************************************************/
/*  Add a row to the symbol table                                     */
/**********************************************************************/
static void addrow(char *fname, toktyp frole, toktyp ftype,
                   int fsize, int faddr)
{
    if (numrows < TABSIZE) {
        set_name(numrows, fname);
        set_role(numrows, frole);
        set_type(numrows, ftype);
        set_size(numrows, fsize);
        set_addr(numrows, faddr);
        numrows++;
    } else {
        printf("\nError: Symbol table full!");
    }
}
/**********************************************************************/
/*  Initialise the symbol table                                       */
/**********************************************************************/
/*
static void initst()
{
    addrow(tok2lex(predef),  typ, predef, 0, 0);
    addrow(tok2lex(undef),   typ, predef, 0, 0);
    addrow(tok2lex(error),   typ, predef, 0, 0);
    addrow(tok2lex(integer), typ, predef, 4, 0);
    addrow(tok2lex(boolean), typ, predef, 4, 0);
    addrow(tok2lex(real),    typ, predef, 8, 0);
}*/
/**********************************************************************/
/*  return a reference to the ST (index) if name found else nfound    */
/**********************************************************************/
static int get_ref(char * fpname)
{
    int i;
    for (i = 0; i < numrows; i++) {
        if (strcmp(get_name(i), fpname) == 0) {
            return i;  // Name found, return index
        }
    }
    return nfound;  // Name not found
}

/**********************************************************************/
/*  PUBLIC METHODS for this OBJECT  (EXPORTED)                        */
/**********************************************************************/
/**********************************************************************/
/*  Display the symbol table                                          */
/**********************************************************************/
static void p_symrow(int ftref)
{
    printf("\n%10s    %5s    %8s    %6d    %8d", 
        get_name(ftref),
        tok2lex(get_role(ftref)),
        tok2lex(get_type(ftref)),
        get_size(ftref),
        get_addr(ftref));
}

void p_symtab()
{
    static int initialized = 0;
    int i, total_size = 0;
    
    // Initialize the symbol table if it hasn't been initialized yet
    if (!initialized) {
        numrows = 0;  // Just initialize the counters
        startp = 0;
        initialized = 1;
        //initst();
    }
    
    printf("\n________________________________________________________ ");
    printf("\n THE SYMBOL TABLE");
    printf("\n________________________________________________________ ");
    printf("\n       NAME       ROLE       TYPE      SIZE      ADDR     ");
    printf("\n________________________________________________________ ");
    
    // Display all entries
    for (i = 0; i < numrows; i++) {
        p_symrow(i);
    }
    
    // Calculate total size - but don't double count
    // If there's a program entry, use its size as the total
    // Otherwise, sum all variable sizes
    if (startp < numrows && get_role(startp) == program) {
        total_size = get_size(startp);
    } else {
        for (i = 0; i < numrows; i++) {
            if (get_role(i) == id) {
                total_size += get_size(i);
            }
        }
    }
    
    printf("\n________________________________________________________ ");
    printf("\n STATIC STORAGE REQUIRED is %d BYTES", total_size);
    printf("\n________________________________________________________ ");
    printf("\n");
}

/**********************************************************************/
/*  Add a program name to the symbol table                            */
/**********************************************************************/
void addp_name(char * fpname)
{
    int ref = get_ref(fpname);
    
    if (ref == nfound) {
        // Not found, add to table
        startp = numrows;  // Remember program position
        addrow(fpname, program, program, 0, 0);  // Initial size is 0
    } else {
        // Already exists, error (not shown in example)
        printf("\nSemantic: Program identifier already declared");
    }
}

/**********************************************************************/
/*  Add a variable name to the symbol table                           */
/**********************************************************************/
void addv_name(char * fpname)
{
    int ref = get_ref(fpname);
    
    if (ref == nfound) {
        // Not found, add to table with default values
        // Type will be set later with setv_type
        addrow(fpname, id, undef, 0, 0);
    } else {
        // Already exists, error
        printf("\nSemantic: Identifier already declared");
    }
}

/**********************************************************************/
/*  Find a name in the the symbol table                               */
/*  return a Boolean (true, false) if the name is in the ST           */
/**********************************************************************/
int find_name(char * fpname)
{
    return (get_ref(fpname) != nfound);
}

/**********************************************************************/
/*  Set the type of an id list in the symbol table                    */
/**********************************************************************/
void setv_type(toktyp ftype)
{
    int i;
    int size_value = 0;
    int current_addr = 0;
    
    // Determine size based on type
    if (ftype == integer || ftype == boolean) {
        size_value = 4;  // 4 bytes for int and boolean
    } else if (ftype == real) {
        size_value = 8;  // 8 bytes for real
    }
    
    // Calculate starting address
    if (startp < numrows) {
        current_addr = get_size(startp);  // Get current program size
    }
    
    // Find all undefined variables and set their type and size
    for (i = 0; i < numrows; i++) {
        if (get_role(i) == id && get_type(i) == undef) {
            set_type(i, ftype);
            set_size(i, size_value);
            set_addr(i, current_addr);
            current_addr += size_value;
        }
    }
    
    // Update program size to reflect total memory needed
    if (startp < numrows) {
        set_size(startp, current_addr);
    }
}

/**********************************************************************/
/*  Get the type of a variable from the symbol table                  */
/**********************************************************************/
toktyp get_ntype(char * fpname)
{
    int ref = get_ref(fpname);
    
    if (ref != nfound) {
        return get_type(ref);
    } else {
        printf("\nSemantic: Identifier not declared");
        return error;
    }
}

/**********************************************************************/
/* End of code                                                        */
/**********************************************************************/
