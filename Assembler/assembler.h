#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_
#include "../Data_structures/Vector/vector.h"
#include "../Data_structures/Trie/trie.h"
#include "../preprocessor/preprocessor.h"
#include "../Lexical_analysis/lexical_analysis.h"

/* A symbol can be: external or entry
* If it is an entry, it must be code or data.
* It can be defined as entry and then be defined as code or data later in the code */
struct symbols{
    enum {
        extrn,/* This is an external symbol that has not been defined outside the program */
        entry,/* This is an internal symbol that has not been defined as code or data and will be defined later */
        code, /* Anything that it's not .data or .string */
        data, /* .data or .string */
        entry_code,/* This is an internal symbol that defined as code */
        entry_data /* This is an internal symbol that defined as data */
    }symbol_type;
    char name_of_symbol[MAX_LABEL_LENGTH + 1];/* the name of the symbol */
    int address_of_symbol; /* the symbol address in the memory */
    int declared_line_symbol;  /* The from_line number where we defined the symbol */

};

/* all the Vector to hold all the data */
struct arrays{
    Vector code;    /*  array that will store all the lines of code */
    Vector data;    /* array that will store all the lines of data */
    Vector symbol_table; /* The symbol table */
    Trie to_search_the_symbol;  /* we will save all the symbols in the Trie, so we can look for them after if we need*/
    Vector array_of_missing_symbols; /* we will save a pointer to each symbol that we couldn't find in the trie, so we can later build a binary code for this symbol */
    Vector extern_addresses; /* array that will store all addresses for extern symbols  */
    int entry_count;
};

/* we will store Vector of Vectors, in each Vector we will have extern symbol + all the address of the extern symbol */
struct extern_symbol{
    char label_extern_name[MAX_LABEL_LENGTH + 1]; /* the name of the extern symbol */
    Vector address_of_extern; /* Vector that will hold all the addresses of the extern symbol */
};

void error_making(int error_number, char *error_output,const char *file_name,int line_count);


/*The main function that calls preproccessor and then Lexical_analysis*/
int assembler(int file_count,char **file_name);

#endif