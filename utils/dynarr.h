#ifndef DYNARR_
#define DYNARR_

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>

typedef struct {

    size_t count;
    size_t capacity;
    size_t type_size;

} DynArr_Header;

static const size_t DEFAULT_CAPACITY = 16;

void *dynarr_create(size_t type_size, size_t capacity);
void dynarr_reserve(void **array_ptr, size_t min_capacity);
static void dynarr_push(void **array_ptr, const void *object);
void dynarr_destroy(void *array);
void dynarr_clear(void *array);

size_t dynarr_peek_capacity(void *array);
size_t dynarr_peek_count(void *array);
static size_t dynarr_pop_index(void *array);

#define dynarr_append(arr, val) ({ \
    __auto_type _temp = (val); \
    dynarr_push((void **)&(arr), &_temp); \
})

// made by ai
#define dynarr_pop(arr) ((arr)[dynarr_pop_index(arr)])

#ifdef DYNARR_IMPLEMENTATION // implementation

void *dynarr_create(size_t type_size, size_t capacity) {
    if (!type_size) { fprintf(stderr, "Fatal Error: No type size has been given in func: %s()\n", __func__); return NULL; }
    if (!capacity) capacity = DEFAULT_CAPACITY;

    void *raw = malloc(sizeof(DynArr_Header) + (type_size * capacity));
    if (!raw) { fprintf(stderr, "Fatal Error: Couldn't allocate for some reasons in func: %s()\n", __func__); return NULL; }

    DynArr_Header *header = (DynArr_Header *)raw;
    header->count = 0;
    header->capacity = capacity;
    header->type_size = type_size;

    return (void *)((char *)raw + sizeof(DynArr_Header));
}

void dynarr_reserve(void **array_ptr, size_t min_capacity) {
    if (!array_ptr || !*array_ptr || !min_capacity) { fprintf(stderr, "Fatal Error: No array pointer or min capacity has been given in func: %s()\n", __func__); return; }

    void *array           = *array_ptr;
    DynArr_Header *header = (DynArr_Header *) ((uint8_t *) array - sizeof(DynArr_Header));
    if (min_capacity <= header->capacity) { printf("Fatal Error (no error actually): You greedy son of a queen, there's enough memory.\n"); return; } // disable this is you're using it for heavy pipeline
    
    void *raw      = realloc(header, sizeof(DynArr_Header) + (min_capacity * header->type_size));
    if (!raw) { fprintf(stderr, "Fatal Error: Couldn't reserve for some reasons in func: %s()\n", __func__); return; }

    header           = (DynArr_Header *) raw;
    header->capacity =  min_capacity;

    *array_ptr = (void *) ((uint8_t *) raw + sizeof(DynArr_Header));
    array      = *array_ptr;
}

static void dynarr_push(void **array_ptr, const void *object) {
    if (!array_ptr || !*array_ptr || !object) { fprintf(stderr, "Fatal Error: No array pointer or object has been given in func: %s()\n", __func__); return; }

    void *array           = *array_ptr;
    DynArr_Header *header = (DynArr_Header *) ((uint8_t *) array - sizeof(DynArr_Header));

    if (header->count == header->capacity) {
        size_t new_cap = (header->capacity) ? header->capacity * 2 : DEFAULT_CAPACITY;
        void *raw      = realloc(header, sizeof(DynArr_Header) + (new_cap * header->type_size));
        if (!raw) { fprintf(stderr, "Fatal Error: Couldn't reallocate for some reasons in func: %s()\n", __func__); return; }

        header           = (DynArr_Header *) raw;
        header->capacity =  new_cap;

        *array_ptr = (void *) ((uint8_t *) raw + sizeof(DynArr_Header));
        array      = *array_ptr;
    }

    void *dest = (uint8_t *)array + (header->count * header->type_size);
    memcpy(dest, object, header->type_size);
    header->count++;
}

void dynarr_destroy(void *array) {
    if (array) {
        // Free the original header block located just before the array payload
        void *raw = (void *)((char *)array - sizeof(DynArr_Header));
        free(raw);
    }
}

void dynarr_clear(void *array) {
    if (!array) { fprintf(stderr, "Fatal Error: Passed an unallocated/NULL pointer in func: %s()\n", __func__); return; }

    DynArr_Header *header = (DynArr_Header *) ((uint8_t *) array - sizeof(DynArr_Header));
    header->count         = 0;
}

size_t dynarr_peek_capacity(void *array) {
    if (!array) { fprintf(stderr, "Fatal Error: Passed an unallocated/NULL pointer in func: %s()\n", __func__); return 0;}

    DynArr_Header *header = (DynArr_Header *) ((uint8_t *) array - sizeof(DynArr_Header));
    return header->capacity;
}

size_t dynarr_peek_count(void *array) {
    if (!array) { fprintf(stderr, "Fatal Error: Passed an unallocated/NULL pointer in func: %s()\n", __func__); return 0;}

    DynArr_Header *header = (DynArr_Header *) ((uint8_t *) array - sizeof(DynArr_Header));
    return header->count;
}

static size_t dynarr_pop_index(void *array) {
    if (!array) { fprintf(stderr, "Fatal Error: Passed an unallocated/NULL pointer in func: %s()\n", __func__); return 0;}
    DynArr_Header *header = (DynArr_Header *) ((uint8_t *) array - sizeof(DynArr_Header));
    if (header->count == 0) return 0;
    
    header->count--;
    return header->count;
}

#endif // implementation end

#endif