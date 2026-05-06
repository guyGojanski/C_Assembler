#ifndef __PREPROCESSOR_H_
#define __PREPROCESSOR_H_

#include "../Data_structures/Vector/vector.h"
#include "../Data_structures/Trie/trie.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libgen.h>
#include <string.h>

/* Defining file extensions and other constants */
#define as_file_ext ".as"
#define am_file_ext ".am"
#define MAX_LENGTH 31
#define MAX_LINES 81  /*the size of the from_line is 80 not include the char '\n'*/
#define MAX_ERROR 100
#define SPACE_CHARS " \v\t\f\r\n"
#define SPACE_CHARS_AND_NULL " \v\t\f\r\n,"

/* Macro to skip spaces in a string pointer */
#define SKIP_SPACE(s) while (*s && isspace(*s)) s++

/* Macro to skip spaces in a string pointer in reverse direction */
#define SKIP_SPACE_REVERSE(s, base) while (*s && isspace(*s) && base != s) s++

/* Macro to skip spaces in a string pointer */
#define SKIP_CHARS(s) while (*s && ((!isspace(*s)) && (*s != ','))) s++

const char *preprocessor(char *file_base_name);

#endif