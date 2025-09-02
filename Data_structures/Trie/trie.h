/**
 * @file trie.h
 * @author guy gojanski
 * @brief the following is the API header for the trie data structure
 * @version 0.1
 * @date 2023-08-24 
 * @copyright Copyright (c) 2023
 */

#ifndef __TRIE_H_
#define __TRIE_H_

#define TRIE_BASE_CHAR ' '

/**
 * @brief Structure defining a trie node, including an array of pointers to next nodes
 */
struct trie_node {
    void * end_of_str_ctx;                   /* Context of the end of a string */
    struct trie_node * next[95];             /* Pointers to next nodes */
};


/**
 * @brief Structure defining a trie, including an array of pointers to next nodes
 */
struct trie {
    struct trie_node * next[95];
};

typedef struct trie * Trie;

/**
 * @brief create a new Trie 
 * @return Trie 
 */
Trie trie();

/**
 * @brief
 * @param trie pointer to the Trie .
 * @param string the string to insert in the trie.
 * @param end_of_str_ctx the string context (it can't be NULL!)
 * @return const char* returns pointer to inserted string ( string )
 */
const char * trie_insert(Trie trie,const char *string,void * end_of_str_ctx);
/**
 * @brief Checks if a string exists in the trie
 * @param trie pointer to the Trie 
 * @param string the string to lookup in the trie.
 * @return void* returns pointer to string context
 */
void * trie_exists(Trie trie,const char *string);

/**
 * @brief deletes a string from the trie
 * @param trie pointer to the Trie 
 * @param string the string to you want to delete
 */
void trie_delete(Trie trie,const char  *string);

/**
 * @brief destro the Trie.
 * @param trie pointer to the Trie 
 */
void trie_destroy(Trie * trie);

#endif