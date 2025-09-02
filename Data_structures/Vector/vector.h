/**
 * @file vector.h
 * @author guy gojanski
 * @brief the following is the API header for the vector data structure
 * @version 0.1
 * @date 2023-08-24
 * @copyright Copyright (c) 2023
 */
#ifndef __VECTOR_H_
#define __VECTOR_H_
#include <stddef.h>
#include <stdlib.h>
#define VECTOR_BEGIN_SIZE 12

/* Structure defining a vector, including function pointers for creation and deletion of items */
struct vector {
    void ** items;                    /* Array of items */
    size_t  pointer_count;            /* Total size of the array */
    size_t  item_count;               /* Current number of items in the array */
    void *  (*ctor)(const void *copy);/* Pointer to function to create a new item */
    void    (*dtor)(void *item);      /* Pointer to function to delete an item */
};
typedef struct vector * Vector;

/**
 * @brief allocates and returns a new Vector data structure.
 * @param creat_vector pointer to user constructor function.
 * @param clean_vector pointer to user destructor function.
 * @return Vector  
 */
Vector new_vector(void * (*creat_vector)(const void *copy),void (*clean_vector)(void *item)); 
 /**
  * @brief inserts a new item to the vector
  * @param v pointer to the vector
  * @param copy_item pointer to the item you wish to push in the vector
  * @return void* vector_insert(Vector v,const void * copy_item)
  */  
void * vector_insert(Vector v,const void * copy_item);

/**
 * @brief returns a pointer to the first pointer of the vector item list.
 * @param v pointer to the vector
 * @return void* const* pointer to the first pointer of the vector item list
 */
void *const *vector_begin(const Vector v);

/**
 * @brief returns a pointer to the last pointer of the vector item list.
 * @param v pointer to the vector
 * @return void* const* pointer to the last pointer of the vector item list
 */
void *const *vector_end(const Vector v);

/**
 * @brief returns how many items there is in the vector
 * @param v pointer to the vector
 * @return size_t is the number of items in the vector
 */
size_t vector_get_item_count(const Vector v);

/**
 * @brief destroy the vector
 * @param v pointer to the vector
 */
void vector_destroy(Vector * v);

/**
 * @brief macro for iterating on a vector.
 */
#define VECTOR_ITERATING(begin,end,v) for(begin = vector_begin(v),end= vector_end(v);begin <= end;begin++)

/**
 * @brief macro for reverse iterating on a vector.
 */
#define VECTOR_ITERATING_REVERSE(begin,end,v) for(begin = vector_begin(v),end= vector_end(v);begin <= end;end--)

#endif