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
 *
 * @param ctor pointer to user constructor function.
 * @param dtor pointer to user destructor function.
 * @return Vector the vector.
 */
Vector new_vector(void * (*creat_vector)(const void *copy),void (*clean_vector)(void *item));

/**
 * @brief inserts a new item to the vector.
 *
 * @param v reference to the vector
 * @param copy_item a reference to the item you wish to push in the vector.
 * @return void* returns pointer to inserted element , or returns NULL pointer if allocation was not sucessfull.
 */
void * vector_insert(Vector v,const void * copy_item);

/**
 * @brief returns a pointer to the first pointer of the vector item list.
 *
 * @param v reference to the vector
 * @return void* const*
 */
void *const *vector_begin(const Vector v);

/**
 * @brief returns a pointer to the last pointer of the vector item list.
 *
 * @param v reference to the vector
 * @return void* const*
 */
void *const *vector_end(const Vector v);

/**
 * @brief returns how many items are actually inside the vector
 *
 * @param v reference to the vector
 * @return size_t
 */
size_t vector_get_item_count(const Vector v);

/**
 * @brief completely deallocates the vector, by calling the dtor function on each item + free'ing the memory of the vector
 *
 * @param v reference to the vector
 */
void vector_destroy(Vector * v);

/**
 * @brief macro for iterating on a vector.
 *
 */
#define VECTOR_ITERATING(begin,end,v) for(begin = vector_begin(v),end= vector_end(v);begin <= end;begin++)

/**
 * @brief macro for reverse iterating on a vector.
 *
 */
#define VECTOR_ITERATING_REVERSE(begin,end,v) for(begin = vector_begin(v),end= vector_end(v);begin <= end;end--)

#endif