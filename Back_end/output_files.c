#include "output_files.h"

#define ENTRY_EXTENSION ".ent"
#define EXTERN_EXTENSION ".ext"
#define OBJECT_EXTENSION ".ob"

/* A function that generates the .ent file */
static void output_unit_entry_file(const char *entry_file_name,Vector symbol_table) {
    FILE *entry_file;
    void *const *begin_vector; /*to iterate over the vector */
    void *const *end_vector;   /*to iterate over the vector */

    entry_file = fopen(entry_file_name, "w");

    if (entry_file != NULL) {
        VECTOR_ITERATING(begin_vector, end_vector, symbol_table) {
            if (*begin_vector != NULL) {
                if ((((struct symbols *) (*begin_vector))->symbol_type == entry_data) || (((struct symbols *) (*begin_vector))->symbol_type == entry_code)) {
                    fprintf(entry_file, "%s\t%u\n", ((struct symbols *) (*begin_vector))->name_of_symbol, ((struct symbols *) (*begin_vector))->address_of_symbol);
                }
            }
        }
        fclose(entry_file);
    }
}
/* A function that generates the .ext file */
static void output_unit_extern_file(const char *extern_file_name,Vector extern_addresses) {
    FILE *extern_file;
    void *const *begin_vector; /*to iterate over the vector */
    void *const *end_vector;   /*to iterate over the vector */
    void *const *begin_address_vector; /*to iterate over the sec vector */
    void *const *end_address_vector;   /*to iterate over the sec vector */
    extern_file = fopen(extern_file_name, "w");
    if (extern_file != NULL) {
        VECTOR_ITERATING(begin_vector, end_vector, extern_addresses) {
            if (*begin_vector != NULL) {
                VECTOR_ITERATING(begin_address_vector, end_address_vector, ((const struct extern_symbol*)(*begin_vector))->address_of_extern){
                    if (*begin_address_vector != NULL) {
                        fprintf(extern_file, "%s\t%u\n", ((const struct extern_symbol*)(*begin_vector))->label_extern_name,*((unsigned int*)(*begin_address_vector)));
                    }
                }
            }
        }
    }
    fclose(extern_file);
}
/* A function that generates the .obj file */
static void base_64_unit(FILE *object_file,Vector memory_section) {
    const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/";

    void *const *begin_vector; /*to iterate over the vector */
    void *const *end_vector;   /*to iterate over the vector */

    unsigned int twelveBits;
    unsigned int sec_sixBits,first_sixBits;

    VECTOR_ITERATING(begin_vector, end_vector, memory_section) {
        if (*begin_vector != NULL){
            twelveBits = *(unsigned int*)(*begin_vector);
            /* Extract the rightmost 6 bits*/
            sec_sixBits = twelveBits & 0x3F;

            /* Shift the 12-bit value right by 6 and then extract the rightmost 6 bits*/
            first_sixBits = (twelveBits >> 6) & 0x3F;
            fprintf(object_file,"%c%c\n", base64chars[first_sixBits], base64chars[sec_sixBits]);

        }
    }
}
/* the main functuin */
void output_unit(const char *file_name,const struct arrays * array) {
    char *entry_file_name;
    char *extern_file_name;
    char *object_file_name;
    FILE *object_file;

    size_t length = strlen(file_name);

    if (array->entry_count > 0) {
        entry_file_name = strcat(strcpy(malloc(length + strlen(ENTRY_EXTENSION) + 1), file_name), ENTRY_EXTENSION);
        output_unit_entry_file(entry_file_name, array->symbol_table);
        free(entry_file_name);

    }
    if (vector_get_item_count(array->extern_addresses) > 0) {
        extern_file_name = strcat(strcpy(malloc(length + strlen(EXTERN_EXTENSION) + 1), file_name), EXTERN_EXTENSION);
        output_unit_extern_file(extern_file_name, array->extern_addresses);
        free(extern_file_name);

    }
    object_file_name = strcat(strcpy(malloc(length + strlen(OBJECT_EXTENSION) + 1), file_name), OBJECT_EXTENSION);
    object_file = fopen(object_file_name,"w");
    if(object_file != NULL){
        fprintf(object_file,"%lu %lu\n", (long unsigned int)vector_get_item_count(array->code), (long unsigned int)vector_get_item_count(array->data));
        base_64_unit(object_file,array->code);
        base_64_unit(object_file,array->data);
        fclose(object_file);
    }
    
    free(object_file_name);
}
