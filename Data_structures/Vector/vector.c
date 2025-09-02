
/* Including necessary libraries and defining a constant for initial size of the vector */
#include "vector.h"

/* Function to create a new vector with specified create and delete functions */
Vector new_vector(void * (*ctor)(const void *copy),void (*dtor)(void *item)) {
    /* Allocate memory for new vector structure */
    Vector new = calloc(1,sizeof(struct vector));
    if(new == NULL)
        return NULL;
    /* Initialize vector size and allocate memory for items array */
    new->pointer_count = VECTOR_BEGIN_SIZE;
    new->items = calloc(VECTOR_BEGIN_SIZE,sizeof(void*));
    if(new->items == NULL) {
        free(new);
        return NULL;
    }
    /* Assign create and delete functions */
    new->ctor = ctor;
    new->dtor = dtor;
    return new;
}

/* Function to insert a new item into a vector, expanding the vector if necessary */
void * vector_insert(Vector v,const void * copy_item) {
    size_t it;
    void ** temp;
    /* If the vector is full, double its size */
    if(v->item_count == v->pointer_count) {
        v->pointer_count *=2;
        temp = realloc(v->items,v->pointer_count * sizeof(void *));
        if(temp == NULL) {
            v->pointer_count /= 2;
            return NULL;
        }
        v->items = temp;
        /* Create new item and add it to vector */
        v->items[v->item_count] = v->ctor(copy_item);
        if(v->items[v->item_count] ==NULL) {
            return NULL;
        }
        v->item_count++;
        /* Initialize remaining new pointers to NULL */
        for(it = v->item_count; it < v->pointer_count; it++) {
            v->items[it] = NULL;
        }
        /* If vector is not full, add item to first available position */
    }else {
        for(it = 0;it<v->pointer_count;it++) {
            if(v->items[it] == NULL) {
                v->items[it] = v->ctor(copy_item);
                if(v->items[it] != NULL) {
                    v->item_count++;
                    break;
                }
                return NULL;
            }
        }
    }
    return v->items[v->item_count-1];
}

/* Function to return a pointer to the first item in a vector */
void *const *vector_begin(const Vector v) {
    return v->items;
}

/* Function to return a pointer to the last item in a vector */
void *const *vector_end(const Vector v) {
    return &v->items[v->pointer_count - 1];
}

/* Function to get the current number of items in a vector */
size_t vector_get_item_count(const Vector v) {
    return v->item_count;
}

/* Function to destroy a vector and free all its resources */
void vector_destroy(Vector * v) {
    size_t it;
    /* If vector is not NULL, delete all items and free array and vector memory */
    if (*v != NULL) {
        if ((*v)->dtor != NULL) {
            /* If the delete function is specified, use it to delete all items in the vector */
            for (it = 0; it < (*v)->pointer_count; it++) {
                if ((*v)->items[it] != NULL)
                    (*v)->dtor((*v)->items[it]);
            }
        }
        /* Free array of items */
        free((*v)->items);
        /* Free vector structure */
        free(*v);
        /* Set pointer to vector to NULL */
        *v = NULL;
    }
}

