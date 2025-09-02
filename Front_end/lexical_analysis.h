/**
 * @file lexical_analysis.h
 * @author guy gojanski
 * @brief the following is the API header for the lexical_analysis
 * @version 0.1
 * @date 2023-08-24 
 * @copyright Copyright (c) 2023
 */

#ifndef __LEXICAL_ANALYSIS_
#define __LEXICAL_ANALYSIS_

#include "../Data_structures/Trie/trie.h"
#include "../Data_structures/Vector/vector.h"
#include "../preprocessor/preprocessor.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libgen.h>
#include <string.h>

#define MAX_LABEL_LENGTH 31

/**
 * @brief the pattern that the lexical_analysis will return to the assembler
 */
struct pattern {
    char warning_message[300];
    char error_message[300];
    /* Label size is a maximum of 31 characters, which ends with ':'. The character ':'
     * is not part of the label size (it's not one of the 31 characters)  */
    char label_name[MAX_LABEL_LENGTH + 1]; /* if we have label in the row*/

    /* its command or directive */
    enum
    {
        pattern_option_directive = 0,
        pattern_option_command = 1,
        pattern_option_error = 2
    } pattern_option;

    union
    {
        /*----------directive----------*/
        struct {/*all the cases for directive choice */
            /*We will match the appropriate union to the appropriate choice in the enum*/
            enum
            {
                directive_extern = 0,
                directive_entry = 1,
                directive_string = 2,
                directive_data = 3
            } directive_enum;
            union
            {
                char label_name[MAX_LINES]; /* Type of label for entry/extern */
                char string[MAX_LINES];    /* pointer to string fir .string */
                struct {          /* array of numbers for data for .data */
                    int data_of_numbers[MAX_LINES];
                    int data_count; /* count the numbers */
                } data_numbers;
            } opperand_option;
        } directive;

        /*----------command----------*/
        struct {  /*all the cases for command choice */
            /*We will match the appropriate union to the appropriate choice in the enum*/
            enum
            {
                /* first group-2 opperand */
                command_mov = 0,
                command_cmp = 1,
                command_add = 2,
                command_sub = 3,
                command_lea = 6,
                /* sec group-1 opperand */
                command_not = 4,
                command_clr = 5,
                command_inc = 7,
                command_dec = 8,
                command_jmp = 9,
                command_bne = 10,
                command_red = 11,
                command_prn = 12,
                command_jsr = 13,
                /* third group-0 opperand */
                command_rts = 14,
                command_stop = 15
            } command_enum;

            /* In both enum and union, the first cell of the array is the source And the second cell is the destination */
            union
            {
                int number;                    /*Addressing method 1 constant number*/
                int register_number;           /*Addressing method 2 label*/
                char label[MAX_LINES];                   /*Addressing method 3 Register*/
            } opperand[2];  /* We have two operands (the source operand and the destination operand) Everyone has their own address */
            /* we build enum, so we can know which opperand addressing we choose*/
            enum
            {
                none = 0, /* 0000 */                 /* There are groups that do not have a target operand, so we will mark it with 0 */
                number = 1, /* 0001*/                /* Addressing method 1 constant number */
                label = 3,  /* 0011*/                /* Addressing method 2 label */
                register_number = 5 /* 0101*/       /* Addressing method 3 Register */
            } addressing_method[2];  /* We have two operands (the source operand and the destination operand) Everyone has their own address */
        } command;
    } option_directive_or_command;
};

/**
 * @brief The method accepts a line from the assembler and returns a pattern
 * @param line line of code
 * @return struct pattern  A template that shows what is in the row
 */
struct pattern lexical_analysis(char  *line);

#endif