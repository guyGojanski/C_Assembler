/**
 * @file output_files.h
 * @author guy gojanski
 * @brief the following is the API header for the output_files
 * @version 0.1
 * @date 2023-08-24 
 * @copyright Copyright (c) 2023
 */

#ifndef PROJECT_IN_C_OUTPUT_FILES_H
#define PROJECT_IN_C_OUTPUT_FILES_H

#include "../Middle_end/assembler.h"

/**
 * @brief  A function that returns us output files: .ent , .ext , .obj
 * @param file_name the name of the correct file
 * @param array all the Vector that hold all the data from the lines
 */
void output_unit(const char *file_name,const struct arrays * array);

#endif
