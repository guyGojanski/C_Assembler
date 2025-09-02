#include "assembler.h"
#include "stdio.h"
#include "../Back_end/output_files.h"
/* macro */
#define IC_START 100

/* COLORS */
#define ANSI_COLOR_RGB(r, g, b) "\x1b[38;2;" #r ";" #g ";" #b "m"
#define ANSI_COLOR_RESET "\x1b[0m"
/* how to use colors     printf(ANSI_COLOR_RGB(240, 142, 51) "This text is LIGHT PURPLE!" ANSI_COLOR_RESET "\n");*/



/* When we finish going through all the code, we will go through the table of missing symbols and
* make sure that every symbol that exists in the table has a suitable binary code */
struct missing_symbol{
    char label_name[MAX_LABEL_LENGTH + 1];
    unsigned int *pointer_to_binary_code;
    unsigned int from_line;
    unsigned int address;

};
/*  constructor */
static void *constructor(const void *copy){
    return memcpy(malloc(sizeof(unsigned int)),copy, sizeof(unsigned int));
}
/*  destructor */
static void destructor(void *item){
    free(item);
}
/*  constructor for symbol */
static void *constructor_symbol(const void *copy){
    return memcpy(malloc(sizeof(struct symbols)),copy, sizeof(struct symbols));
}
/*  destructor for symbol */
static void destructor_symbol(void *item){
    free(item);
}
/*  constructor for extern_addresses */
static void *constructor_extern_addresses(const void *copy)
{
    return memcpy(malloc(sizeof(struct extern_symbol)),copy, sizeof(struct extern_symbol));
}
/*  destructor for extern_addresses */
static void destructor_extern_addresses(void *item){
    struct extern_symbol *new = item;
    vector_destroy(&new->address_of_extern);
    free(item);
}
/*  constructor for missing_symbol */
static void *constructor_binary_code_of_missing_symbol(const void *copy){
    return memcpy(malloc(sizeof(struct missing_symbol)), copy, sizeof(struct missing_symbol));
}
/*  destructor for missing_symbol */
static void destructor_binary_code_of_missing_symbol(void *item) {
    free(item);
}
/*  1 means An error was found in one of the lines.
*  2 means a warning was found in on of the lines */
void error_making(int error_number, char *error_output,const char *file_name,int line_count)
{

    if(error_number == 1)
    {
        printf("%s Line:%d ", file_name,line_count);
        printf(ANSI_COLOR_RGB(255, 0, 0) "error:  " ANSI_COLOR_RESET); /* RED */
        printf("[%s]  \n", error_output);
        return;
    }
    else if(error_number == 2)
    {
        printf("%s Line:%d ", file_name,line_count);
        printf(ANSI_COLOR_RGB(255, 151, 54) "warning:  " ANSI_COLOR_RESET); /* ORANGE */
        printf("[%s]  \n", error_output);
        return;
    }
}
/* this function will add new extern or will add an address call for existed extern*/
static void add_extern_to_Vector(Vector extern_addresses,const char *extern_name ,const unsigned int address)
{
    struct extern_symbol new ={0};
    void *const *begin;
    void *const *end;
    VECTOR_ITERATING(begin,end,extern_addresses)
    {
        if(*begin != NULL){
            if(strcmp(extern_name,((const struct extern_symbol*)(*begin))->label_extern_name)==0){
                vector_insert(((const struct extern_symbol*)(*begin))->address_of_extern,&address);
                return;
            }
        }
    }
    strcpy(new.label_extern_name,extern_name);
    new.address_of_extern = new_vector(constructor,destructor);
    vector_insert(new.address_of_extern,&address);
    vector_insert(extern_addresses,&new);
}
/* return: 0 if the compilation passed  */
static int compiler(FILE *am_file, struct arrays * array,const char* file_name) {

    int flag_error = 0; /* 0 - mean good, 1- mean error 2-mean error warning */
    int line_count = 1; /* count the number of the from_line*/
    unsigned int *pointer_to_label;
    int i;

    unsigned int extern_call_address;
    unsigned int binary_code = 0;

    char *letter;
    char line_sent_to_lexer[MAX_LINES + 1] = {0};
    char buffer[MAX_ERROR + 1] ={0};

    struct pattern new_pattern;   /* the pattern of the from_line that the lexical return me*/
    struct symbols *lookForStringInTrie;
    struct symbols symbols_temporary = {0}; /* temporary symbols to store are symbol*/
    struct missing_symbol binary_code_of_missing_symbol = {0};
    struct symbols *sym ;
    struct missing_symbol *sym_n ;

    void *const *begin_vector; /*to iterate over the vector */
    void *const *end_vector;   /*to iterate over the vector */


    while (fgets(line_sent_to_lexer, sizeof(line_sent_to_lexer), am_file))
    {
        new_pattern = lexical_analysis(line_sent_to_lexer); /* pattern from the lexical analysis */
        /*--------------------------Error--------------------------*/
        if (new_pattern.pattern_option == pattern_option_error)
        {
            /* If the flag is changed to 1, the assembler will not continue the program and will stop after reading all the lines
            * add a flag so if we have an error, create an AM file that will contain all the error
            * we found that look like gcc patter error.*/
            /* if we have error */
            if (new_pattern.error_message[0] != '\0')
            {
                error_making(1, new_pattern.error_message, file_name, line_count);
                flag_error = 1;
                line_count++;
                continue;
            }
                /* if we have warning*/
            else if (new_pattern.warning_message[0] != '\0')
            {
                error_making(2, new_pattern.warning_message, file_name, line_count);
                flag_error = 2;
            }
        }

        /* we have label name*/
        if (new_pattern.label_name[0] != '\0')
        {
            /* we don't work on the "new_pattern", so we build a new one calls symbols_temporary 
            * we enter symbols_temporary the name of the symbol*/
            
            strcpy(symbols_temporary.name_of_symbol, new_pattern.label_name); 

            /* check if the label name we have, if it's in the symbol Trie */
            lookForStringInTrie = trie_exists(array->to_search_the_symbol, symbols_temporary.name_of_symbol);

            /*--------------------------command--------------------------*/
            if (new_pattern.pattern_option == pattern_option_command)
            {
                /* we have a symbol */
                if (lookForStringInTrie != NULL) {
                    /* meaning the symbol already been defined before and its ERROR*/
                    if (lookForStringInTrie->symbol_type != entry)
                    {
                        error_making(1, "The symbol already been defined before", file_name, line_count);
                        flag_error = 1;
                        line_count++;
                        continue;
                    }

                        /* meaning the symbol is "entry" */
                    else
                    {
                        lookForStringInTrie->address_of_symbol = (int) vector_get_item_count(array->code) + IC_START;/* the size of the array code + 100 */
                        lookForStringInTrie->symbol_type = entry_code;
                    }
                }
                    /* we have a symbol, but it doesn't exist in the Trie, so we use a temporary symbol */
                else
                {
                    /*we enter the type and address*/
                    symbols_temporary.address_of_symbol = (int) vector_get_item_count(array->code) + IC_START;/* the size of the array code + 100 */
                    symbols_temporary.symbol_type = code;
                    symbols_temporary.declared_line_symbol = line_count;
                    trie_insert(array->to_search_the_symbol, symbols_temporary.name_of_symbol, vector_insert(array->symbol_table, &symbols_temporary));
                }
            }

                /*--------------------------directive--------------------------*/
            else if (new_pattern.pattern_option == pattern_option_directive)
            {
                /* we have a symbol */
                if (lookForStringInTrie != NULL)
                {
                    /* if we have .data or .string it must be an entry */
                    if ((new_pattern.option_directive_or_command.directive.directive_enum == directive_string) || (new_pattern.option_directive_or_command.directive.directive_enum == directive_data))
                    {
                        /* meaning the symbol already been defined before and its ERROR*/
                        if (lookForStringInTrie->symbol_type != entry)
                        {
                            error_making(1, "The symbol already been defined before", file_name, line_count);
                            flag_error = 1;
                            line_count++;
                            continue;
                        }
                        else
                        {
                            lookForStringInTrie->address_of_symbol = (int) vector_get_item_count(array->data); /* DC */
                            lookForStringInTrie->symbol_type = entry_data;
                        }
                    }
                }

                    /* we have a symbol, but it doesn't exist in the Trie, so we use a temporary symbol */
                else
                {
                    if (new_pattern.option_directive_or_command.directive.directive_enum == directive_entry)
                    {
                        symbols_temporary.symbol_type = entry;
                    }
                    else if (new_pattern.option_directive_or_command.directive.directive_enum == directive_extern)
                    {
                        symbols_temporary.symbol_type = extrn;
                    }
                    else
                    {
                        symbols_temporary.symbol_type = data;
                        symbols_temporary.address_of_symbol = (int) vector_get_item_count(array->data); /* DC */
                        symbols_temporary.declared_line_symbol = line_count;
                        trie_insert(array->to_search_the_symbol, symbols_temporary.name_of_symbol, vector_insert(array->symbol_table, &symbols_temporary));
                    }
                }
            }
        }
        switch (new_pattern.pattern_option)
        {
            case pattern_option_command:
            {
                /* int is 32 bits, and we only need 12 bits, so we will only work on
                * the rightmost 12 bits each time (so that all 20 leftmost bits will always be 0) */
                binary_code = new_pattern.option_directive_or_command.command.addressing_method[1] << 2;
                binary_code |= new_pattern.option_directive_or_command.command.addressing_method[0] << 9;
                binary_code |= new_pattern.option_directive_or_command.command.command_enum << 5;
                vector_insert(array->code, &binary_code);

                switch (new_pattern.option_directive_or_command.command.command_enum)
                {
                    /* we have 2 operands*/
                    case command_mov:
                    case command_cmp:
                    case command_add:
                    case command_sub:
                    case command_lea:
                        /* If we have two operands that are registers, then we will build one binary code from both */
                        if ((new_pattern.option_directive_or_command.command.addressing_method[0] == 5) && (new_pattern.option_directive_or_command.command.addressing_method[1] == 5))
                        {
                            binary_code = new_pattern.option_directive_or_command.command.opperand[1].register_number << 2;
                            binary_code |= new_pattern.option_directive_or_command.command.opperand[0].register_number << 7;
                            vector_insert(array->code, &binary_code);
                        }
                            /* Otherwise, each operand will have its own binary code */
                        else
                        {
                            for (i = 0; i < 2; i++)
                            {
                                /* if we have a number */
                                if (new_pattern.option_directive_or_command.command.addressing_method[i] == number)
                                {
                                    binary_code = new_pattern.option_directive_or_command.command.opperand[i].number << 2;
                                    vector_insert(array->code, &binary_code);
                                }

                                    /* if we have a register number */
                                else if (new_pattern.option_directive_or_command.command.addressing_method[i] == register_number)
                                {
                                    if (i == 0)
                                    {
                                        binary_code = new_pattern.option_directive_or_command.command.opperand[0].register_number << 7;
                                    }
                                    else
                                    {
                                        binary_code = new_pattern.option_directive_or_command.command.opperand[1].register_number << 2;
                                    }
                                    vector_insert(array->code, &binary_code);
                                }

                                    /* if we have a label */
                                else if (new_pattern.option_directive_or_command.command.addressing_method[i] == label)
                                {
                                    lookForStringInTrie = trie_exists(array->to_search_the_symbol, new_pattern.option_directive_or_command.command.opperand[i].label);

                                    if ((lookForStringInTrie != NULL) && (lookForStringInTrie->symbol_type != entry)) 
                                    {
                                        binary_code = lookForStringInTrie->address_of_symbol << 2;  
                                        if (lookForStringInTrie->symbol_type == extrn) {
                                            binary_code |= 1;
                                            extern_call_address = vector_get_item_count(array->code) + IC_START;
                                            add_extern_to_Vector(array->extern_addresses, lookForStringInTrie->name_of_symbol, extern_call_address);
                                        }
                                        else
                                        {
                                            binary_code |= 2;
                                        }
                                    }

                                    /*  is a pointer which points to the binary_code which we have now entered into code */
                                    pointer_to_label = vector_insert(array->code, &binary_code);
                                    if ((lookForStringInTrie == NULL) || ((lookForStringInTrie != NULL) && (lookForStringInTrie->symbol_type == entry)))  
                                    {               
                                        strcpy(binary_code_of_missing_symbol.label_name, new_pattern.option_directive_or_command.command.opperand[i].label);
                                        binary_code_of_missing_symbol.pointer_to_binary_code = pointer_to_label;
                                        binary_code_of_missing_symbol.from_line = line_count;
                                        extern_call_address = vector_get_item_count(array->code) + IC_START - 1 ;
                                        binary_code_of_missing_symbol.address = extern_call_address;
                                        vector_insert(array->array_of_missing_symbols, &binary_code_of_missing_symbol);
                                    }
                                }

                                    /* none */
                                else
                                    break;
                            }
                        }
                        break;

                        /*we have 1 operand*/
                    case command_not:
                    case command_clr:
                    case command_inc:
                    case command_dec:
                    case command_jmp:
                    case command_bne:
                    case command_red:
                    case command_prn:
                    case command_jsr:
                        /* each operand will have its own binary code */
                        /* if we have a number */
                        if (new_pattern.option_directive_or_command.command.addressing_method[1] == number)
                        {
                            binary_code = new_pattern.option_directive_or_command.command.opperand[1].number << 2;
                            vector_insert(array->code, &binary_code);
                        }
                            /* if we have a register number */
                        else if (new_pattern.option_directive_or_command.command.addressing_method[1] == register_number)
                        {
                            binary_code = new_pattern.option_directive_or_command.command.opperand[1].register_number << 2;
                            vector_insert(array->code, &binary_code);
                        }

                            /* if we have a label */
                        else if (new_pattern.option_directive_or_command.command.addressing_method[1] == label)
                        {
                            lookForStringInTrie = trie_exists(array->to_search_the_symbol, new_pattern.option_directive_or_command.command.opperand[1].label);
                            if ((lookForStringInTrie != NULL) && (lookForStringInTrie->symbol_type != entry))           
                            {
                                binary_code = lookForStringInTrie->address_of_symbol << 2;
                                if (lookForStringInTrie->symbol_type == extrn)
                                {
                                    binary_code |= 1;
                                    extern_call_address = vector_get_item_count(array->code) + IC_START;
                                    add_extern_to_Vector(array->extern_addresses, lookForStringInTrie->name_of_symbol, extern_call_address);
                                }
                                else
                                {
                                    binary_code |= 2;
                                }
                            }
                            /* pointer_to_label is a pointer which points to the binary_code which we have now entered into code */
                            pointer_to_label = vector_insert(array->code, &binary_code);
                            if ((lookForStringInTrie == NULL) || ((lookForStringInTrie != NULL) && (lookForStringInTrie->symbol_type == entry)))      
                            {
                                strcpy(binary_code_of_missing_symbol.label_name, new_pattern.option_directive_or_command.command.opperand[1].label);
                                binary_code_of_missing_symbol.pointer_to_binary_code = pointer_to_label;
                                binary_code_of_missing_symbol.from_line = line_count;
                                extern_call_address = vector_get_item_count(array->code) + IC_START - 1 ;
                                binary_code_of_missing_symbol.address = extern_call_address;
                                vector_insert(array->array_of_missing_symbols, &binary_code_of_missing_symbol);
                            }
                        }
                        break;

                        /*we have 0 operands*/
                    case command_rts:
                    case command_stop:
                        /* in this case, we do nothing because we have 1 word only */
                        break;
                }
                break;
            }

            case pattern_option_directive:
            {
                if ((new_pattern.option_directive_or_command.directive.directive_enum == directive_entry) || (new_pattern.option_directive_or_command.directive.directive_enum == directive_extern))
                {
                    lookForStringInTrie = trie_exists(array->to_search_the_symbol, new_pattern.option_directive_or_command.directive.opperand_option.label_name);
                    if (lookForStringInTrie != NULL)
                    {
                        if (new_pattern.option_directive_or_command.directive.directive_enum == directive_entry)
                        {
                            if ((lookForStringInTrie->symbol_type == entry) || (lookForStringInTrie->symbol_type == entry_data) || (lookForStringInTrie->symbol_type == entry_code))
                            {
                                error_making(2, "Redefinition of a symbol", file_name, line_count);
                                flag_error = 2;
                            }
                            else if (lookForStringInTrie->symbol_type == data)
                                lookForStringInTrie->symbol_type = entry_data;

                            else if (lookForStringInTrie->symbol_type == code)
                                lookForStringInTrie->symbol_type = entry_code;

                            else if (lookForStringInTrie->symbol_type == extrn)
                            {
                                error_making(1, "the symbol as being declared as extern and now being declared as entry", file_name, line_count);
                                flag_error = 1;
                                line_count++;
                                continue;
                            }
                        }

                        else if (new_pattern.option_directive_or_command.directive.directive_enum == directive_extern)
                        {
                            if (lookForStringInTrie->symbol_type == extrn)
                            {
                                error_making(2, "Redefinition of a symbol", file_name, line_count);
                                flag_error = 2;
                            }
                            else
                            {
                                error_making(1, "An extern symbol cannot be set after it has been set to another definition", file_name, line_count); 
                                flag_error = 1;
                                line_count++;
                                continue;
                            }
                        }
                    }
                    else 
                    {
                        strcpy(symbols_temporary.name_of_symbol, new_pattern.option_directive_or_command.directive.opperand_option.label_name);
                        if (new_pattern.option_directive_or_command.directive.directive_enum == 1) 
                        {
                            symbols_temporary.symbol_type = entry;
                        }
                        if (new_pattern.option_directive_or_command.directive.directive_enum == 0) {
                            symbols_temporary.symbol_type = extrn;
                        }
                        
                        symbols_temporary.declared_line_symbol = line_count;
                        symbols_temporary.address_of_symbol = 0;
                        trie_insert(array->to_search_the_symbol, symbols_temporary.name_of_symbol, vector_insert(array->symbol_table, &symbols_temporary));
                    }
                }

                else if (new_pattern.option_directive_or_command.directive.directive_enum == directive_string)
                {
                    letter = new_pattern.option_directive_or_command.directive.opperand_option.string;
                    while (*letter != '\0')
                    {
                        binary_code = (unsigned char) *letter;
                        vector_insert(array->data, &binary_code);
                        letter++;
                    }
                    binary_code = 0;
                    vector_insert(array->data, &binary_code);
                }
                else if (new_pattern.option_directive_or_command.directive.directive_enum == directive_data)
                {
                    for (i = 0; i < new_pattern.option_directive_or_command.directive.opperand_option.data_numbers.data_count; i++)
                    {
                        vector_insert(array->data, &new_pattern.option_directive_or_command.directive.opperand_option.data_numbers.data_of_numbers[i]);
                    }
                }
                break;
            }
            case pattern_option_error:
                break;

        }
        line_count++;

    }

    /* We will go through the symbol table and look for a symbol that has been initialized but not defined.
     * If we find it, we will return a detailed error */
    VECTOR_ITERATING(begin_vector, end_vector, array->symbol_table) {
        if (*begin_vector != NULL)
        {
            sym = (struct symbols *) (*begin_vector);
            if (sym->symbol_type == entry)
            {
                printf(ANSI_COLOR_RGB(255, 0, 0) "error:  " ANSI_COLOR_RESET); /* RED */
                sprintf(buffer, "Symbol %s, declared at line %d, remains undefined in the %s file.", sym->name_of_symbol, sym->declared_line_symbol, file_name);
                printf("[%s]  \n", buffer);
                flag_error = 1;
            }
            else if((sym->symbol_type == entry_data) || (sym->symbol_type == entry_code))
                array->entry_count++;
            if((sym->symbol_type == entry_data) || (sym->symbol_type == data)) {
                sym->address_of_symbol += vector_get_item_count(array->code) + IC_START;
            }
        }
    }

    VECTOR_ITERATING(begin_vector, end_vector, array->array_of_missing_symbols) {
        if (*begin_vector != NULL) {
            sym_n = (struct missing_symbol *) (*begin_vector);
            lookForStringInTrie = trie_exists(array->to_search_the_symbol, sym_n->label_name);
            if (lookForStringInTrie != NULL)
            {
                switch (lookForStringInTrie->symbol_type) {
                    case extrn:
                        *(sym_n->pointer_to_binary_code) = 1;
                        add_extern_to_Vector(array->extern_addresses, lookForStringInTrie->name_of_symbol, sym_n->address);
                        break;

                    case entry:
                        /*noting*/
                        break;

                    case entry_data:
                    case entry_code:
                    case code:
                    case data:
                        *(sym_n->pointer_to_binary_code) = lookForStringInTrie->address_of_symbol << 2;
                        *(sym_n->pointer_to_binary_code) |= 2;
                        break;
                }
            }
            else
            {
                /*ERROR MISSING SYMBOL*/
                printf(ANSI_COLOR_RGB(255, 0, 0) "error:  " ANSI_COLOR_RESET); /* RED */
                sprintf(buffer, "Symbol %s referenced at line %d not found in the symbol table",sym_n->label_name,sym_n->from_line);
                printf("[%s]  \n", buffer);
                flag_error = 1;
            }
        }
    }
    return flag_error;
}
/* A function that creates all vectors */
static struct arrays build_obj_file(){
    struct arrays create_arrays = {0};
    create_arrays.code = new_vector(constructor,destructor);
    create_arrays.data = new_vector(constructor,destructor);
    create_arrays.symbol_table = new_vector(constructor_symbol,destructor_symbol);
    create_arrays.array_of_missing_symbols = new_vector(constructor_binary_code_of_missing_symbol,destructor_binary_code_of_missing_symbol);
    create_arrays.extern_addresses = new_vector(constructor_extern_addresses,destructor_extern_addresses);
    create_arrays.to_search_the_symbol = trie();
    return create_arrays;
}
/* A function that destroy all vectors */
static void  destroy_obj_file(struct arrays * array){
    vector_destroy(&array->code);
    vector_destroy(&array->data);
    vector_destroy(&array->symbol_table);
    vector_destroy(&array->array_of_missing_symbols);
    vector_destroy(&array->extern_addresses);
    trie_destroy(&array->to_search_the_symbol);
}


/* the main function*/
int assembler(int file_count,char **file_name) {
    FILE *am_file;
    int i;
    const char *am_file_name;

    struct arrays obj_file;

    for (i = 0; i < file_count; i++)
    {
        am_file_name = preprocessor(file_name[i]);   
        if (am_file_name)
        {
            am_file = fopen(am_file_name, "r");
            if(am_file)
            {     
                obj_file = build_obj_file();
                /* 0 mean no warning or error, 2 mean we have some warning */
                
                if(compiler(am_file,&obj_file,am_file_name) != 1)
                {
                    output_unit(file_name[i],&obj_file);
                }
                else if(compiler(am_file, &obj_file,am_file_name) == 1)
                {
                    printf("Do not produce an output file, but only produce an am file\n");                
                }
                
                fclose(am_file); 
                destroy_obj_file(&obj_file);
            }
            free((void*)am_file_name);
        }
    }
    return 1;
}



