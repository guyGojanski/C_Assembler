/* Including necessary libraries and defining a constant for base character of the trie */
#include "trie.h"
#include <stdlib.h>


static struct trie_node *internal_trie_exists(struct trie_node * node_i,const char * string) {
    while(node_i) {
        if(*string == '\0' && node_i->end_of_str_ctx != NULL) {
            return node_i;
        }
        node_i = node_i->next[(*string) - TRIE_BASE_CHAR];
        string++;
    }
    return NULL;
}

/* Function to create a new trie */
Trie trie() {
/* Allocate memory for new trie structure */
    return calloc(1,sizeof(struct trie));
}

/* Function to insert a string into a trie */
const char *trie_insert(Trie trie,const char *string,void * end_of_str_ctx) {
    /* Start with the first node of the trie */
    struct trie_node ** iterator = &trie->next[(*string) - TRIE_BASE_CHAR];
    /* Iterate over the string */
    while(1) {
        /* If the node does not exist, create it */
        if(*iterator == NULL) {
            (*iterator) = calloc(1,sizeof(struct trie_node));
        /* If the node cannot be created, return NULL */
            if(*iterator == NULL)
                return NULL;
        }
        /* Move to the next character */
        string++;
        /* If the string is not over, move to the next node */
        if(*string !='\0')
            iterator = &(*iterator)->next[(*string) - TRIE_BASE_CHAR];
            /* If the string is over, break */
        else
            break;
    }
    /* Set the context of the end of the string */
    (*iterator)->end_of_str_ctx = end_of_str_ctx;
    return string;
}

/* Function to delete a string from a trie */
void trie_delete(Trie trie,const char *string) {
    struct trie_node * find_node;
    /* If the string is NULL, do nothing */
    if(string == NULL)
        return;
    /* Find the node corresponding to the string */
    find_node = internal_trie_exists(trie->next[(*string) - TRIE_BASE_CHAR],string+1);
    /* If the node exists, clear the context of the end of the string */
    if(find_node)
        find_node->end_of_str_ctx = NULL;
}

/* Function to check if a string exists in a trie */
void * trie_exists(Trie trie,const char *string) {
    struct trie_node *find_node;
    /* If the string is NULL, return NULL */
    if (string == NULL)
        return NULL;
    /* Find the node corresponding to the string */
    find_node = internal_trie_exists(trie->next[(*string) - TRIE_BASE_CHAR], string + 1);
    /* If the node does not exist, return NULL, otherwise return the context of the end of the string */
    return find_node == NULL ? NULL : find_node->end_of_str_ctx;
}

/* Internal function to destroy a sub-trie from a specified node */
static void trie_destroy_sub(struct trie_node * node_i) {
    int i;
    /* Iterate over all next nodes */
    for(i=0;i<95;i++) {
        /* If a next node exists, destroy it and set the pointer to NULL */
        if(node_i->next[i] != NULL) {
            trie_destroy_sub(node_i->next[i]);
            node_i->next[i] = NULL;
        }
    }
    /* Free the current node */
    free(node_i);
}

/* Function to destroy a trie and free all its resources */
void trie_destroy(Trie * trie) {
    int i;
    /* If the trie is not NULL, destroy all its nodes */
    if (*trie != NULL) {
        Trie t = *trie;
        /* Iterate over all next nodes */
        for (i = 0; i < 95; i++) {
            /* If a next node exists, destroy it */
            if (t->next[i] != NULL)
                trie_destroy_sub(t->next[i]);
        }
        /* Free the trie */
        free(*trie);
        /* Set the pointer to the trie to NULL */
        (*trie) = NULL;
    }
}
