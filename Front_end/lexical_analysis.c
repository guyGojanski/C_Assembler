#include "lexical_analysis.h"

#define my_strdup strcpy(malloc(strlen(s1) +1 ), s1)
typedef struct pattern(*command_cases_func)(char *directive_word,char *line);
/* build tree tries */
static Trie command_trie = NULL;
static Trie directive_trie = NULL;
static Trie register_trie = NULL;
int trie_build = 0;
char error_label[MAX_ERROR];
struct pattern new_pattern = {0};


/* A function that receives a source/destination operand and checks
 * if it is a label/register/number or not set properly
 * return: 1 - if a number 3-if its label 5- if its register*/
static int check_operand(char *operand) {
    int length = strlen(operand);
    int flag_not_digit = 0;
    int flag_not_label = 0;
    int i;    

    /* check if operand is a whole number */
    for (i = 0; i < length; i++) {
        if ((operand[i] == '+' || operand[i] == '-') && i == 0) {
            continue;
        }
        if (!isdigit(operand[i])) {
            flag_not_digit = 1;
            break;
        }
    }

    /* It's a number */
    if (flag_not_digit == 0) {
        return 1;
    }    
    /* check if It's a register */
    if ((length == 3) && (operand[0] == '@'))
    {
        if(*(operand+1) == 'r')
        {
            if (operand[2] >= '1' && operand[2] <= '8') {
                return 5;
            }
        }
    }

    /* check if It's a label */
    if (isalpha(operand[0])) { /* Check if the first character is a letter*/
        for (i = 1; i < length; i++) {
            if (!isalnum(operand[i])) { /* Check if other characters are alphanumeric*/
                flag_not_label = 1;
                break;
            }
        }
        if (flag_not_label == 0) {
            return 3;
        }
    }

    return 0;
}
/*
 * A function that receives a label and checks whether it is a reserved word of the language
 * and whether it is syntactically correct.
 * return -  "The error type" if the label is a reserved word of the language otherwise returns "label"
 */
static char *check_label(char *label) {
    if ((trie_exists(command_trie, label)) != NULL) {
        sprintf(error_label,"The word %s cannot be a instruction word Because it is a reserved word of the language", label);
        return error_label;
    } else if ((trie_exists(directive_trie, label)) != NULL) {
        sprintf(error_label,"The word %s cannot be a directive word Because it is a reserved word of the language", label);
        return error_label;
    } 
    else if ((trie_exists(register_trie, label)) != NULL) {
        sprintf(error_label,"The word %s cannot be a register number Because it is a reserved word of the language", label);
        return error_label;
    }
    if (check_operand(label) == 3)
        return "label";
    else
    {        
        sprintf(error_label,"The word %s is not a reserved word of the language but it is not a valid label", label);
        return error_label;
    }
}
/*
 * The function receives a source operand, a destination operand and addressing method(number,label,register)
 * and enters the appropriate details into the pattern
 */
static void inserting_operand_pattern(char *source_operand,char *destination_operand,int addressing_number){
    char *temp;
    int num;
    if(source_operand != NULL){
        switch (addressing_number)
        {
            case 1:
                new_pattern.option_directive_or_command.command.addressing_method[0] = number;
                new_pattern.option_directive_or_command.command.opperand[0].number = atoi(source_operand);
                break;
            case 3:
                new_pattern.option_directive_or_command.command.addressing_method[0] = label;
                strcpy(new_pattern.option_directive_or_command.command.opperand[0].label, source_operand);  
                break;
            case 5:
                new_pattern.option_directive_or_command.command.addressing_method[0] = register_number;
                temp = source_operand +2 ;
                num = atoi(temp);
                new_pattern.option_directive_or_command.command.opperand[0].register_number = num ;
                break;
        }
    }
        /* destination_opperand */
    if(destination_operand != NULL){
        switch (addressing_number){
            case 1:
                new_pattern.option_directive_or_command.command.addressing_method[1] = number;
                new_pattern.option_directive_or_command.command.opperand[1].number = atoi(destination_operand);
                break;
            case 3:
                new_pattern.option_directive_or_command.command.addressing_method[1] = label;
                strcpy(new_pattern.option_directive_or_command.command.opperand[1].label, destination_operand);                
                break;
            case 5:
                new_pattern.option_directive_or_command.command.addressing_method[1] = register_number;
                temp = destination_operand +2 ;
                num = atoi(temp);
                new_pattern.option_directive_or_command.command.opperand[1].register_number = num;
                break;
        }
    }

}
/*
 * The function receives a line and checks whether it contains exactly two operands, the type of each of which corresponds to the opcode
 */
static struct pattern operand_2(char *command_word,char *line) {
    /* mov/cmp/add/sub/lea */
    char *source_operand = NULL;
    char *start_source_operand = NULL;
    char *end_source_operand = NULL;
    char *destination_operand = NULL;
    char *start_destination_operand = NULL;
    char *end_destination_operand = NULL;
    char *temp, *comma;
    char error[MAX_LINES];
    char *check1,*check2;
    int length;

    if (*line == '\0')
    {
        new_pattern.pattern_option = pattern_option_error;
        sprintf(error,"Missing operand -opcode %s must have exactly 2 operands",command_word);
        strcpy(new_pattern.error_message,error );
        return new_pattern;
    }

    comma = strchr(line, ',');
    /*check if we have "," in the line*/
    if (comma == NULL){
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "The character ',' must appear in the line, and it must appear after the first operand");
        return new_pattern;
    }
    /*check for more than 1 "," in the line*/
    if ((strchr(line, ',')) != (strrchr(line, ','))) {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "The character ',' must appear exactly once and it need to be after the first operand");
        return new_pattern;
    }

    /* fine the first operand and then the sec one*/
    if ((comma != NULL) && (line < comma))
    {
        /* pointer to the first char in the first operand */
        start_source_operand = line;
        /* temp must exist because we checked earlier if "," exists in the sentence, */
        temp = strpbrk(line, SPACE_CHARS_AND_NULL);

        /* if temp == NULL, it can be a Potential Segmentation Fault lead to undefined behavior */
        if(temp == NULL)
        {
            new_pattern.pattern_option = pattern_option_error;
            strcpy(new_pattern.error_message, "An unexpected error occurred while parsing the operand");
            return new_pattern;
        }



        end_source_operand = temp - 1;
        /* -------------------------------------------we have the first operand-------------------------------------------*/
        /* copy the word to the operand */
        length = (int) (end_source_operand - start_source_operand + 1);
        source_operand = (char *) malloc(sizeof(char) * (length) + 1);
        strncpy(source_operand, line, length);
        source_operand[(length)] = '\0';

        line = temp;
        SKIP_SPACE(line);

        if (line != comma)
        {
            new_pattern.pattern_option = pattern_option_error;
            strcpy(new_pattern.error_message, "The character ',' must appear after the first operand");
            free(source_operand);
            return new_pattern;
        }
        else
        {
            /* line == comma */
            line = line + 1;
            SKIP_SPACE(line);
            if(*line == '\0')
            {
                new_pattern.pattern_option = pattern_option_error;
                sprintf(error,"Missing operand -opcode %s must have exactly 2 operands ",command_word);
                strcpy(new_pattern.error_message,error );
                free(source_operand);
                return new_pattern;

            }

            /* pointer to the first char in the sec operand */
            start_destination_operand = line;
            temp = strpbrk(line, SPACE_CHARS);
            if (temp == NULL)
                end_destination_operand = line + strlen(line) -1;
            else
                end_destination_operand = temp - 1;

            /* -------------------------------------------we have the sec operand-------------------------------------------*/
            /* copy the word to the operand */
            length = (int) (end_destination_operand - start_destination_operand + 1);
            destination_operand = (char *) malloc(sizeof(char) * (length) + 1);
            strncpy(destination_operand, line, length);
            destination_operand[(length)] = '\0';
            line = end_destination_operand + 1;
            SKIP_SPACE(line);
            if (*line != '\0')
            {
                new_pattern.pattern_option = pattern_option_error;
                sprintf(error,"Too many operands - opcode %s must have exactly 2 operands",command_word);
                strcpy(new_pattern.error_message,error);
                free(source_operand);
                free(destination_operand);
                return new_pattern;
            }
        }
    }

    else
    {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "The character ',' must appear after the first operand");
        free(source_operand);
        free(destination_operand);
        return new_pattern;
    }

    /* last checks */
        check1 = source_operand + 1;
        if((source_operand[0] == '@') && (*check1 == 'r')) {
            if (check_operand(source_operand) != 5) {
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message,
                       "In the language there are only the registers @r1,...,@r8 - source operand");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
            }
        }
        check2 = destination_operand + 1;

        if((destination_operand[0] == '@') && (*check2 == 'r')) {
            if (check_operand(destination_operand) != 5) {
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message,
                       "In the language there are only the registers @r1,...,@r8 -destination operand ");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
            }
        }


    if ((destination_operand[strlen(destination_operand) - 1] == ':') || (source_operand[strlen(source_operand) - 1] == ':') ){
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "An operand cannot be a label definition");
        free(source_operand);
        free(destination_operand);
        return new_pattern;
    }




    /*------There are two operands------*/
    new_pattern.pattern_option = pattern_option_command;
    /* If I got here I have exactly two operands, and now we will check their correctness */
    if ((strcmp(command_word, "mov") == 0) || (strcmp(command_word, "add") == 0) || (strcmp(command_word, "sub") == 0))
    {
        if (strcmp(command_word, "mov") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_mov;
        else if (strcmp(command_word, "add") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_add;
        /* strcmp(command_word, "sub") */
        else
            new_pattern.option_directive_or_command.command.command_enum = command_sub;

        /* return: 1 - if a number 3-if its label 5- if its register */
        /* source_opperand-"1,3,5" */
        switch (check_operand(source_operand)) {
            case 1:
                if((atoi(source_operand) > 511) || (atoi(source_operand) < -511)){
                    /*error number is too big*/
                    new_pattern.pattern_option = pattern_option_error;
                    sprintf(error,"The number %d is too big ,the range that can be coded is between -511 to 511",atoi(source_operand));
                    strcpy(new_pattern.error_message,error);
                    free(source_operand);
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(source_operand,NULL,1);
                break;
            case 3:
                /* check if the label is not a save word */
                if (strcmp(check_label(source_operand), "label") != 0)
                {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message, check_label(source_operand));
                    free(source_operand);
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(source_operand, NULL, 3);
                break;

            case 5:
                inserting_operand_pattern(source_operand,NULL,5);
                break;
            case 0:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The source operand can only be immediate addressing (integer),"
                                                  " direct addressing (label) or direct addressing a register (register)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
        }

        /* destination_opperand-3,5" */
        switch (check_operand(destination_operand)) {

            case 1:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The destination operand cannot be in an addressing method of 001 (integer)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
            case 0:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The destination operand can only be direct addressing (label) or direct addressing a register (register)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;


            case 3:
                /* check if the label is not a save word */
                if (strcmp(check_label(destination_operand), "label") != 0) {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message, check_label(destination_operand));
                    free(source_operand);
                    free(destination_operand);
                    return new_pattern;
                }
                else
                    inserting_operand_pattern(NULL, destination_operand , 3);
                break;
            case 5:
                inserting_operand_pattern(NULL, destination_operand , 5);
                break;
        }
        free(source_operand);
        free(destination_operand);
        return new_pattern;
    }

    else if (strcmp(command_word, "cmp") == 0) {
        /* source_opperand-"1,3,5"    destination_opperand-1,3,5" */
        new_pattern.option_directive_or_command.command.command_enum = command_cmp;

        /* return: 1 - if a number 3-if its label 5- if its register */
        /* source_opperand-"1,3,5" */
        switch (check_operand(source_operand)) {
            case 1:
                if((atoi(source_operand) > 511) || (atoi(source_operand) < -511)){
                    /*error number is too big*/
                    new_pattern.pattern_option = pattern_option_error;
                    sprintf(error,"The number %d is too big ,the range that can be coded is between -511 to 511",atoi(source_operand));
                    strcpy(new_pattern.error_message,error);
                    free(source_operand);
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(source_operand,NULL,1);
                break;
            case 3:
                /* check if the label is not a save word */
                if (strcmp(check_label(source_operand), "label") != 0) {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message, check_label(source_operand));
                    free(source_operand);
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(source_operand,NULL,3);
                break;
            case 5:
                inserting_operand_pattern(source_operand,NULL,5);
                break;
            case 0:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The source operand can only be immediate addressing (integer), "
                                                  "direct addressing (label) or direct addressing a register (register)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
        }

        /* destination_opperand-"1,3,5" */
        switch (check_operand(destination_operand)) {
            case 1:
                if((atoi(destination_operand) > 511) || (atoi(destination_operand) < -511)){
                    /*error number is too big*/
                    new_pattern.pattern_option = pattern_option_error;
                    sprintf(error,"The number %d is too big ,the range that can be coded is between -511 to 511",atoi(destination_operand));
                    strcpy(new_pattern.error_message,error);
                    free(source_operand);
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(NULL,destination_operand,1);
                break;
            case 3:
                /* check if the label is not a save word */
                if (strcmp(check_label(destination_operand), "label") != 0) {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message, check_label(destination_operand));
                    free(source_operand);
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(NULL,destination_operand,3);
                break;
            case 5:
                inserting_operand_pattern(NULL,destination_operand,5);
                break;
            case 0:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The destination operand can only be immediate addressing (integer),"
                                                  " direct addressing (label) or direct addressing a register (register)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
        }
        free(source_operand);
        free(destination_operand);
        return new_pattern;
    }

    /* command_word == "lea" */
    else
    {
        /* source_opperand-"3"    destination_opperand-3,5" */
        new_pattern.option_directive_or_command.command.command_enum = command_lea;

        /* return: 1 - if a number 3-if its label 5- if its register */
        /* source_opperand-"3" */

        switch (check_operand(source_operand)) {
            case 1:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The source operand cannot be in an addressing method of 001 (integer)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
            case 3:
                /* check if the label is not a save word */
                if (strcmp(check_label(source_operand), "label") != 0) {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message, check_label(source_operand));
                    free(source_operand);
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(source_operand,NULL,3);
                break;
            case 5:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The source operand cannot be in an addressing method of 101 (register)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
            case 0:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The source operand  can only be a direct address (label)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
        }
        /* destination_opperand-3,5" */
        switch (check_operand(destination_operand)) {
            case 1:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The destination operand cannot be in an addressing method of 001 (integer)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
            case 3:
                /* check if the label is not a save word */
                if (strcmp(check_label(destination_operand), "label") != 0) {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message, check_label(destination_operand));
                    free(source_operand);
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(NULL,destination_operand,3);

                break;
            case 5:
                inserting_operand_pattern(NULL,destination_operand,5);
                break;
            case 0:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The destination operand can only be direct addressing (label) or direct addressing a register (register)");
                free(source_operand);
                free(destination_operand);
                return new_pattern;
        }
        free(source_operand);
        free(destination_operand);
        return new_pattern;
    }
}
/*
 * The function receives a line and checks whether it contains only a single operand whose type matches the opcode
 */
static struct pattern operand_1(char *command_word,char *line) {
    /* not/clr/inc/dec/jmp/bne/red/prn/jsr */
    char *destination_operand, *temp;
    char *start_destination_operand = NULL;
    char *end_destination_operand = NULL;
    char error[MAX_LINES];
    int length;
    char *check;

    if (*line == '\0') {
        new_pattern.pattern_option = pattern_option_error;
        sprintf(error, "Missing operand - opcode %s must have exactly 1 operand", command_word);
        strcpy(new_pattern.error_message, error);
        return new_pattern;
    }

    /*check if we have "," in the line*/
    if (strchr(line, ',') != NULL) {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "The character ',' cant appear in the line");
        return new_pattern;
    }


    /* pointer to the first char in the first operand */
    start_destination_operand = line;
    temp = strpbrk(line, SPACE_CHARS);
    if (temp != NULL)
        end_destination_operand = temp - 1;
    else
        end_destination_operand = line + strlen(line) - 1;


    /*-------------------------------------------we have the first operand-------------------------------------------*/
    /* copy the word to the operand */
    length = (int) (end_destination_operand - start_destination_operand + 1);
    destination_operand = (char *) malloc(sizeof(char) * (length) + 1);
    strncpy(destination_operand, line, length);
    destination_operand[(length)] = '\0';

    if (temp != NULL)
    {
        SKIP_SPACE(temp);
        /* check if we have exactly one operand */
        if (*temp != '\0') {
            new_pattern.pattern_option = pattern_option_error;
            sprintf(error, "opcode %s must have exactly 1 operand", command_word);
            strcpy(new_pattern.error_message, error);
            free(destination_operand);
            return new_pattern;
        }
    }

    if (destination_operand[strlen(destination_operand) - 1] == ':') {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "An operand cannot be a label definition");
        free(destination_operand);
        return new_pattern;
    }
    check = destination_operand + 1;
    if ((destination_operand[0] == '@') && (*check == 'r'))
    {
        if (check_operand(destination_operand) != 5) {
            new_pattern.pattern_option = pattern_option_error;
            strcpy(new_pattern.error_message, "In the language there are only the registers @r1,...,@r8 ");
            free(destination_operand);
            return new_pattern;
        }
    }

    new_pattern.pattern_option = pattern_option_command;
    new_pattern.option_directive_or_command.command.addressing_method[0] = none;
    if (strcmp(command_word, "prn") == 0)
    {
        new_pattern.option_directive_or_command.command.command_enum = command_prn;
        /* destination_opperand-"1,3,5" */
        switch (check_operand(destination_operand))
        {
            case 1:
                if((atoi(destination_operand) > 511) || (atoi(destination_operand) < -511)){
                    /*error number is too big*/
                    new_pattern.pattern_option = pattern_option_error;
                    sprintf(error,"The number %d is too big ,the range that can be coded is between -511 to 511",
                            atoi(destination_operand));
                    strcpy(new_pattern.error_message,error);
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(NULL, destination_operand, 1);
                break;
            case 3:
                /* check if the label is not a save word */
                if (strcmp(check_label(destination_operand), "label") != 0) {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message, check_label(destination_operand));
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(NULL, destination_operand, 3);
                break;
            case 5:
                inserting_operand_pattern(NULL, destination_operand, 5);
                break;
            case 0:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "An operand can only be an immediate address (integer), a direct address (label) or a direct register address (register)");
                free(destination_operand);
                return new_pattern;
        }
        free(destination_operand);
        return new_pattern;
    }

    /* command_word == not/clr/inc/dec/jmp/bne/red/jsr */
    else
    {
        if (strcmp(command_word, "not") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_not;
        else if (strcmp(command_word, "clr") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_clr;
        else if (strcmp(command_word, "inc") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_inc;
        else if (strcmp(command_word, "dec") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_dec;
        else if (strcmp(command_word, "jmp") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_jmp;
        else if (strcmp(command_word, "bne") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_bne;
        else if (strcmp(command_word, "red") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_red;
        else if (strcmp(command_word, "jsr") == 0)
            new_pattern.option_directive_or_command.command.command_enum = command_jsr;


        /* destination_opperand-3,5" */
        switch (check_operand(destination_operand))
        {
            case 1:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The destination operand cannot be in an addressing method of 001 (Immediate address - whole number)");
                free(destination_operand);
                return new_pattern;
            case 0:
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "An operand can only be a direct address(label) or direct register(register)");
                free(destination_operand);
                return new_pattern;
            case 3:
                /* check if the label is not a save word */
                if (strcmp(check_label(destination_operand), "label") != 0) {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message, check_label(destination_operand));
                    free(destination_operand);
                    return new_pattern;
                }
                inserting_operand_pattern(NULL, destination_operand, 3);
                break;
            case 5:
                inserting_operand_pattern(NULL, destination_operand, 5);
                break;
        }
        free(destination_operand);
        return new_pattern;
    }
}
/*
 * A function that accepts stop or rts and checks whether it is the last word in the line
 */
static struct pattern operand_0(char *command_word,char *line) {
    /* stop,rts */
    char error[MAX_LINES];
        if (*line != '\0') {
        new_pattern.pattern_option = pattern_option_error;
        sprintf(error, "opcode %s cannot contain characters after it", command_word);
        strcpy(new_pattern.error_message, error);
        return new_pattern;
    }

    new_pattern.pattern_option = pattern_option_command;
    new_pattern.option_directive_or_command.command.addressing_method[0] = 0;
    new_pattern.option_directive_or_command.command.addressing_method[1] = 0;
    if (strcmp(command_word, "stop") == 0)
        new_pattern.option_directive_or_command.command.command_enum = command_stop;
    else if (strcmp(command_word, "rts") == 0)
        new_pattern.option_directive_or_command.command.command_enum = command_rts;
    return new_pattern;
}
/*
 * A function that accepts data line and add all the valid numbers in to Vector(array)
 */
static struct pattern data(char *directive_word,char *line){
    char *start_number = NULL;
    char *end_number = NULL;
    char *number = NULL;
    char *temp;
    int length;
    int amount_of_numbers = 0;
    char error[MAX_LINES ];
    if (*line == '\0')
    {
        new_pattern.pattern_option = pattern_option_error;
        sprintf(error, " %s must have exactly 1 parameter", directive_word);
        strcpy(new_pattern.error_message, error);
        return new_pattern;
    }

    while(*line != '\0')
    {
        SKIP_SPACE(line);
        start_number = line;
        SKIP_CHARS(line);
        end_number = line - 1;

        /* -------------------------------------------build the number-------------------------------------------*/
        /* copy the word to the operand */
        length = (int) (end_number - start_number + 1);
        number = (char *) malloc(sizeof(char) * (length) + 1);
        strncpy(number, start_number, length);
        number[(length)] = '\0';

        if((check_operand(number)) == 1)
        {
            if((atoi(number) > 2047) || (atoi(number) < -2047)){
                /*error number is too big*/
                new_pattern.pattern_option = pattern_option_error;
                sprintf(error,"The number %d  is too big ,the range that can be coded is between -2047 to 2047",atoi(number));
                strcpy(new_pattern.error_message,error);
                free(number);
                return new_pattern;
            }
            new_pattern.option_directive_or_command.directive.opperand_option.data_numbers.data_of_numbers[amount_of_numbers] = atoi(number);
            amount_of_numbers ++;
            temp = end_number + 1;
            SKIP_SPACE(temp);
            if(*temp == ',')
            {
                line = temp + 1;
                SKIP_SPACE(line);
                if(*line == ',')
                {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message,"you cant have more then 1 comma after a number" );
                    free(number);
                    return new_pattern;
                }
                else if(*line == '\0')
                {
                    new_pattern.pattern_option = pattern_option_error;
                    strcpy(new_pattern.error_message,"cannot be comma after the last number" );
                    free(number);
                    return new_pattern;
                }
            }
            else if (*temp == '\0')
                line = temp;
            else
            {
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message,"A single comma must appear between every two numbers" );
                free(number);
                return new_pattern;
            }
            free(number);
        }
        else
        {
            /*error not a number*/
            new_pattern.pattern_option = pattern_option_error;
            sprintf(error,"The string %s  is not a number",number);
            strcpy(new_pattern.error_message,error);
            free(number);
            return new_pattern;
        }
    }    
    if(strcmp(directive_word,".data") == 0){
        new_pattern.pattern_option = pattern_option_directive;
        new_pattern.option_directive_or_command.directive.opperand_option.data_numbers.data_count = amount_of_numbers;
        new_pattern.option_directive_or_command.directive.directive_enum = directive_data;
    }
    return new_pattern;
}
/*
 * A function that accepts string and checks whether the string is valid
 */
static int check_string(char *string){
    unsigned long length;
    int i;

    length = strlen(string);  
    for (i = 1; i < length - 1 ; i++)
    {
        /*Characters which cannot be printed*/
        if((string[i] < 31) || (string[i] == 127) || (string[i] == '"'))
            return 0;
    }
    return 1;
}
/*
 * A function that accepts .string and checks whether the parameter is a valid string
 */
static struct pattern string(char *directive_word, char *line) {
    char *first_quotation_mark;
    char *sec_quotation_mark;
    char *end;
    char extracted_string[MAX_LINES]; 
    
    /* Check for empty line*/
    if (line == NULL || *line == '\0') {
        new_pattern.pattern_option = pattern_option_error;
        sprintf(new_pattern.error_message, " %s must have exactly 1 parameter", directive_word);
        return new_pattern;
    }

    first_quotation_mark = strchr(line, '"');
    sec_quotation_mark = strchr(first_quotation_mark + 1, '"');
    
    if (first_quotation_mark == NULL){
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "A valid string contains 2 quotation marks - missing quotation marks");
        return new_pattern;
    }

    /* Ensure no characters exist after the closing quotation mark*/
    end = sec_quotation_mark + 1;
    SKIP_SPACE(end);
    if (*end != '\0') {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "A definition of a string is in the following form \"string\"");
        return new_pattern;
    }

    /*Extract string*/
    *sec_quotation_mark = '\0';
    first_quotation_mark++;
    strcpy(extracted_string, first_quotation_mark);

    if (check_string(extracted_string) == 1) {
        new_pattern.pattern_option = pattern_option_directive;
        new_pattern.option_directive_or_command.directive.directive_enum = directive_string;
        strcpy(new_pattern.option_directive_or_command.directive.opperand_option.string, extracted_string);
        return new_pattern;
    } else {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "not a valid string");
        return new_pattern;
    }
}
/*
 * A function that accepts entry or extern and checks whether the parameter is a valid label
 */
static struct pattern entry_or_extern(char *directive_word,char *line){
    int length;
    char *start_label, *end_label, *base, *label, *temp;

    memset(&new_pattern, 0, sizeof(new_pattern));
    if (*line == '\0')
    {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "missing parameter");
        return new_pattern;
    }
    temp = line;
    SKIP_CHARS(temp);
    SKIP_SPACE(temp);
    if(*temp != '\0'){
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "must have exactly 1 parameter and not more");
        return new_pattern;
    }

    SKIP_SPACE(line);
    start_label = line;
    end_label = start_label + strlen(start_label) - 1;
    base = start_label;
    SKIP_SPACE_REVERSE(end_label, base);



    /* -------------------------------------------build the label-------------------------------------------*/
    /* copy the string */
    length = (int) (end_label - start_label + 1);
    label = (char *) malloc(sizeof(char) * (length) + 1);
    strncpy(label, start_label, length);
    label[(length)] = '\0';


    if (strcmp(check_label(label), "label") == 0) {
        if (strcmp(directive_word, ".entry") == 0) {
            new_pattern.pattern_option = pattern_option_directive;
            new_pattern.option_directive_or_command.directive.directive_enum = directive_entry;
            strcpy(new_pattern.option_directive_or_command.directive.opperand_option.label_name, label); 
            free(label);
            return new_pattern; 
        }
        else if(strcmp(directive_word, ".extern") == 0)
        {
            new_pattern.pattern_option = pattern_option_directive;
            new_pattern.option_directive_or_command.directive.directive_enum = directive_extern;
            strcpy(new_pattern.option_directive_or_command.directive.opperand_option.label_name, label); 
            free(label);
            return new_pattern; 
        }
        else{
            new_pattern.pattern_option = pattern_option_error;
            strcpy(new_pattern.error_message, "its not entry and not extern");
            free(label);
            return new_pattern;
        }
    }
    else {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, check_label(label));
        free(label);
        return new_pattern;
    }
}
/* A structure that contains the name of the command, the number of the command (necessary for the enum),
 * a string that is the first operand, a string that is the second operand,
 * If we don't have any operand then it will be NULL */
struct command_table_of_cases {
    const char *command_name; /* the name of the command */
    command_cases_func func;
}command_table_of_cases[16] ={
        /* first group-2 opperand */
        {"mov",operand_2},
        {"cmp",operand_2},
        {"add",operand_2},
        {"sub",operand_2},
        {"lea",operand_2},
        /* sec group-1 opperand */
        {"not",operand_1},
        {"clr",operand_1},
        {"inc",operand_1},
        {"dec",operand_1},
        {"jmp",operand_1},
        {"bne",operand_1},
        {"red",operand_1},
        {"prn",operand_1},
        {"jsr",operand_1},
        /* third group-0 opperand */
        {"rts",operand_0},
        {"stop",operand_0},
};
/* A structure that contains the name of the directive words*/
struct directive_table_of_cases{
    const char *directive_name; /* the name of the command */
    command_cases_func func;
}directive_table_of_cases[4] = {
        {".extern",entry_or_extern},
        {".entry",entry_or_extern},
        {".string",string},
        {".data",data},
};
/* A structure that contains the name of the register numbers*/
struct register_case{
    const char *register_name; /* the name of the command */
}register_case[8]={
        {"@r1"},
        {"@r2"},
        {"@r3"},
        {"@r4"},
        {"@r5"},
        {"@r6"},
        {"@r7"},
        {"@r8"},
};
/* A function that inserts all the structures:
 * command_table_of_cases/directive_table_of_cases/register_case
 * into the appropriate tries
*/
static void trie_insert_command_table_of_operands() {
    int i;
    command_trie = trie();
    directive_trie = trie();
    register_trie = trie();
    for (i = 0; i < 16; i++)
        trie_insert(command_trie, command_table_of_cases[i].command_name, &command_table_of_cases[i]);
    for (i = 0; i < 4 ; i++)
        trie_insert(directive_trie,directive_table_of_cases[i].directive_name , &directive_table_of_cases[i]);
    for (i = 0; i < 8 ; i++)
        trie_insert(register_trie,register_case[i].register_name , &register_case[i]);
    trie_build =1;
}
/* A function that destroy all the structures:
 * command_table_of_cases/directive_table_of_cases/register_case
 */
static void lexical_delete_trie(){
    trie_build =0;
    trie_destroy(&directive_trie);
    trie_destroy(&command_trie);
    trie_destroy(&register_trie);
}

/* The main function that parses the line */
struct pattern lexical_analysis(char *line) {
    int length;
    int length_line;

    char *token, *space, *end;
    char *label;
    char *directive_or_instruction;

    struct command_table_of_cases *temp_command;
    struct directive_table_of_cases *temp_directive;
    struct register_case *temp_register;
    struct pattern result = {0};

    lexical_delete_trie();
    memset(&new_pattern,0,sizeof(struct pattern));
    line[strcspn(line, "\r\n")] = 0;
    /* check if the length of a line is more than 80 chars*/
    length_line = (int) strlen(line);
    if (length_line > MAX_LINES) {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message,
               "The line is too long, A line in are language can have a maximum of 80 characters");
        return new_pattern;
    }
    if (trie_build == 0) {
        /* We add all the options of the opcode/directive/register to each one to the appropriate trie */
        trie_insert_command_table_of_operands();
    }
    /*check for more than 1 ":"*/
    if ((strchr(line, ':')) != (strrchr(line, ':'))) {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "The character ':' cannot appear more than once in a line ");
        return new_pattern;
    }



    /* clear all the spaces before the first word */
    SKIP_SPACE(line);

    /*LABEL: */
    /* Look for the char ":" in the input line */
    token = strchr(line, ':');
    /*have  LABEL */
    if (token != NULL)
    {
        /* We will check if there is a space before the first occurrence of ":" */
        space = strpbrk(line, SPACE_CHARS);
        if ((space != NULL) && (space < token)) {
            new_pattern.pattern_option = pattern_option_error;
            /* check if the first word is a label (meaning the character ":" is attached to the first word) */
            strcpy(new_pattern.error_message, "The definition of the label is incorrect, the character ':' is not attached to the first word");
           return new_pattern;
        }

        /* copy the first word to "label" */
        length = (int) (token - line);
        label = (char *) malloc(sizeof(char) * (length + 1));
        strncpy(label, line, length );
        label[(length)] = '\0';


        if (check_operand(label) != 3) {
            new_pattern.pattern_option = pattern_option_error;
            strcpy(new_pattern.error_message, "A valid label begins with an uppercase or lowercase followed by some series of letters or numbers");
            free(label);
            return new_pattern;

        }
        /* check if the label is not a save word */
        if (strcmp(check_label(label), "label") != 0) {
            new_pattern.pattern_option = pattern_option_error;
            strcpy(new_pattern.error_message, check_label(label));
            free(label);
            return new_pattern;
        }
        /* we have a good label*/
        strcpy(new_pattern.label_name, label);
        line = token + 1;
        SKIP_SPACE(line);

        /* We have only a label definition */
        if (*line == '\0') {
            new_pattern.pattern_option = pattern_option_error;
            strcpy(new_pattern.error_message, "A line cannot contain only a label");
            free(label);
            return new_pattern;
        }
        free(label);
    }

    end = line;
    SKIP_CHARS(end); /* point to the first space */
    /* end point to the last char in the word while line point to the first char in the word*/
    end = end - 1;

    /* copy the word */
    length = (int) (end - line + 1);
    directive_or_instruction = (char *) malloc(sizeof(char) * (length) + 1);
    strncpy(directive_or_instruction, line, length);
    directive_or_instruction[(length)] = '\0';
    line = end + 1;
    SKIP_SPACE(line);

    /* to check if directive_or_instruction is command or directive*/
    temp_command = trie_exists(command_trie, directive_or_instruction);
    if (temp_command != NULL) {
        new_pattern.pattern_option = pattern_option_command;
        result = temp_command->func(directive_or_instruction, line);
        free(directive_or_instruction);
        return result;

    }
    temp_directive = trie_exists(directive_trie, directive_or_instruction);
    if (temp_directive != NULL) {
        new_pattern.pattern_option = pattern_option_directive;
        result = temp_directive->func(directive_or_instruction, line);
        free(directive_or_instruction);
        return result;
    }
    temp_register = trie_exists(register_trie, directive_or_instruction);
    if (temp_register != NULL) {
        new_pattern.pattern_option = pattern_option_error;
        strcpy(new_pattern.error_message, "A register must appear after an opcode");
        free(directive_or_instruction);
        return new_pattern;
    }
    /* We will check the next word, because it is possible that the current word is a label, but they forgot the character ":" */
    else 
    {
        free(directive_or_instruction);
        /* The line definition is missing  */
        if (*line == '\0') {
            new_pattern.pattern_option = pattern_option_error;
            strcpy(new_pattern.error_message, "The line is not a valid line in the language ");
            return new_pattern;
        } 
        else 
        {
            end = line;
            SKIP_CHARS(end); /* point to the first space */
            /* end point to the last char in the word while line point to the first char in the word*/
            end = end - 1;

            /* copy the word */
            length = (int) (end - line + 1);
            directive_or_instruction = (char *) malloc(sizeof(char) * (length) + 1);
            strncpy(directive_or_instruction, line, length);
            directive_or_instruction[(length)] = '\0';

            /* to check if directive_or_instruction is command or directive*/
            if ((trie_exists(directive_trie, directive_or_instruction)) || (trie_exists(command_trie, directive_or_instruction))) {
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The first word is a label, but to define a label you must attach ':' to the first word ");
                free(directive_or_instruction);
                return new_pattern;
            } 
            else {
                new_pattern.pattern_option = pattern_option_error;
                strcpy(new_pattern.error_message, "The line is not a valid line in the language ");
                free(directive_or_instruction);
                return new_pattern;
            }
        }
    }
}



