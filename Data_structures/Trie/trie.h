#ifndef __TRIE_H_
#define __TRIE_H_

#define TRIE_BASE_CHAR ' '

/* Structure defining a trie node, including an array of pointers to next nodes */
struct trie_node {
    void * end_of_str_ctx;                   /* Context of the end of a string */
    struct trie_node * next[95];             /* Pointers to next nodes */
};

/* Structure defining a trie, including an array of pointers to next nodes */
struct trie {
    struct trie_node * next[95];
};


typedef struct trie * Trie;

/**
 * @brief create and returns a new Trie data structure.
 *
 * @return Trie
 */
Trie trie();

/**
 * @brief
 *
 * @param trie reference to the Trie structure.
 * @param string the string to insert in the trie.
 * @param end_of_str_ctx the string context (a pointer that gives a meaning to this string, WARN: should not be NULL!)
 * @return const char* returns pointer to inserted string ( string )
 */
const char * trie_insert(Trie trie,const char *string,void * end_of_str_ctx);

/**
 * @brief
 *
 * @param trie
 * @param string
 * @return void* returns pointer to trie context
 */
void * trie_exists(Trie trie,const char *string);

/**
 * @brief deletes a string in the Trie data structure.
 *
 * @param trie reference to the Trie structure.
 * @param string the string to insert in the trie.
 */
void trie_delete(Trie trie,const char  *string);

/**
 * @brief completely deallocates all memory for the Trie.
 *
 * @param trie
 */
void trie_destroy(Trie * trie);

#endif