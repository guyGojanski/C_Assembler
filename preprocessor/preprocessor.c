#include "preprocessor.h"

/* Structure to store a mcro, containing its name and lines */
struct mcro
{
    char mcro_name[MAX_LENGTH];
    Vector lines;
};
/* Function to create a new from_line, copying from an existing one */
static void *create_new_line(const void *copy)
{
    const char *line = copy;
    return strcpy(malloc(strlen(line) + 1), line);
}
/* Function to free the memory of a from_line */
static void free_line(void *item) {
    free(item);
}
/* Function to create a new mcro, copying from an existing one */
static void *create_new_mcro(const void *copy)
{
    const struct mcro *copy1 = copy;
    struct mcro *new_mcro = malloc(sizeof( struct mcro));
    strcpy(new_mcro->mcro_name, copy1->mcro_name);
    new_mcro->lines = new_vector(create_new_line, free_line);
    return new_mcro;
}
/* Function to free the memory of a mcro */
static void free_mcro(void *item)
{
    struct mcro *mcro = item;
    vector_destroy(&mcro->lines);
    free(mcro);
}
/* An enumeration type to represent various types of from_line detections */
enum preprocess_line_type
{
    empty_line,
    comment_line,
    mcro_definition_successful,
    endmcro_definition_successful,
    any_other_line,
    error_mcro_definition,
    mcro_already_exists_in_trie,
    error_endmcro_definition,
    mcro_call,
    bad_mcro_call
};
/* Function to preprocess and check each from_line of a given mcro */
enum preprocess_line_type preprocess_check_line(char *line, struct mcro **mcro, Trie mcro_lookup, Vector mcro_table) {
    struct mcro new_mcro = {0};
    struct mcro *local_mcro;
    char *token, *temp;

    /* Skip spaces */
    SKIP_SPACE(line);
    /* Find the first position of ';' and change the pointer to '\0' */
    if (line[0] == ';') 
        return comment_line;
  
    /* The from_line is empty, or the from_line was full of spaces, and previously we skipped all of them until the last character '\0'*/
    if (*line == '\0') 
        return empty_line;


    /*------------endmcro------------*/
    token = strstr(line, "endmcro");
    if (token != NULL) 
    {
        temp = token;
        SKIP_SPACE_REVERSE(temp, line);
        if (temp != line) 
            return error_endmcro_definition;
        token += strlen("endmcro");
        SKIP_SPACE(token);
        if (*token != '\0') 
            return error_endmcro_definition;
        *mcro = NULL;
        return endmcro_definition_successful;
    }

    /*------------mcro------------*/
    token = strstr(line, "mcro");
    if (token != NULL) 
    {
        temp = token;
        SKIP_SPACE_REVERSE(temp, line);
        if (temp != line) 
            return error_mcro_definition;
        token += strlen("mcro");
        SKIP_SPACE(token);
        line = token;  
        token = strpbrk(line, SPACE_CHARS);
        if (token != NULL) 
        {
            *token = '\0';
            token++;
            SKIP_SPACE(token);
            if (*token != '\0') 
                return error_mcro_definition;
        }
        *mcro = trie_exists(mcro_lookup, line);
        if (*mcro != NULL)        
            return mcro_already_exists_in_trie;
    
        strcpy(new_mcro.mcro_name, line);
        *mcro = vector_insert(mcro_table, &new_mcro);
        trie_insert(mcro_lookup, line, (*mcro));
        return mcro_definition_successful;
    }

    /*------------ Check if the from_line is a call to a mcro ------------*/
    token = strpbrk(line,SPACE_CHARS);
    if(token)
        *token = '\0';
    local_mcro = trie_exists(mcro_lookup,line);
    if(local_mcro == NULL){
        *token = ' ';
        return any_other_line;
    }
    token++;
    SKIP_SPACE(token);
    if(*token != '\0'){
        return bad_mcro_call;
    }
    *mcro = local_mcro;
    return mcro_call; 
}
/* Function to preprocess an input file and write to an output file */
const char *preprocessor(char *file_base_name)
{
    char *am_file_name;
    char *as_file_name;
    char line_buff[MAX_LINES] = {0};

    FILE *am_file;
    FILE *as_file;
    Vector mcro_table = NULL;
    Trie mcro_table_lookup = NULL;
    struct mcro *mcro = NULL;

    void *const *begin;
    void *const *end;
    int line_count = 1;
    size_t file_base_name_len;

    file_base_name_len = strlen(file_base_name);

    /* Constructing files names */
    as_file_name = strcat(strcpy(malloc(file_base_name_len + strlen(as_file_ext) + 1), file_base_name), as_file_ext);
    am_file_name = strcat(strcpy(malloc(file_base_name_len + strlen(am_file_ext) + 1), file_base_name), am_file_ext); 

    /* Open input file */
    as_file = fopen(as_file_name, "r");
    /* Open output file */
    am_file = fopen(am_file_name, "w");

    if ((am_file == NULL) || (as_file == NULL)) {
        free(as_file_name);
        free(am_file_name);
        return NULL;
    }

    /* Create a vector to store mcros and a trie for lookups */
    mcro_table = new_vector(create_new_mcro, free_mcro);
    mcro_table_lookup = trie();

    /* Read and process each from_line in the input file */
    while (fgets(line_buff, sizeof(line_buff), as_file)) 
    {
        switch (preprocess_check_line(line_buff, &mcro, mcro_table_lookup, mcro_table)) 
        {

            case empty_line:
            case comment_line:
            case mcro_already_exists_in_trie:
            case mcro_definition_successful:
            case endmcro_definition_successful:
                /* SKIP THE LINE*/
                break;

            case mcro_call:
                /* COPY THE CONTENTS OF THE MACRO*/
                VECTOR_ITERATING(begin, end, mcro->lines) {
                    if (*begin != NULL) {
                        fputs((const char *) (*begin), am_file);
                    }
                }
                mcro = NULL;
                break;

            case error_mcro_definition:
            case error_endmcro_definition:
            case bad_mcro_call:
            case any_other_line:
                /* COPY THE LINE*/
                if (mcro != NULL) {
                    vector_insert(mcro->lines, &line_buff);
                } else {
                    fputs(line_buff, am_file);
                }
                break;
        }
        line_count++;
    }
    /* Cleanup */
    vector_destroy(&mcro_table);
    trie_destroy(&mcro_table_lookup);
    free(as_file_name);
    fclose(as_file);
    fclose(am_file);
    return am_file_name;
}
