#pragma once

#include "defines.h"

#define VECTOR_DEFAULT_CAPACITY 1
#define VECTOR_RESIZE_FACTOR 2

enum { VECTOR_CAPACITY, VECTOR_LENGTH, VECTOR_STRIDE, VECTOR_FIELD_LENGTH };

void *_vectorCreate(u64 length, u64 stride);
void _vectorDestroy(void *array);

u64 _vectorFieldGet(void *array, u64 field);
void _vectorFieldSet(void *array, u64 field, u64 value);

void *_vectorResize(void *array);

void *_vectorPush(void *array, const void *value_ptr);
void _vectorPop(void *array, void *dest);

#define vectorCreate(type) _vectorCreate(VECTOR_DEFAULT_CAPACITY, sizeof(type))
#define vectorDestroy(array) _vectorDestroy(array);

#define vectorPush(array, value)                                               \
  {                                                                            \
    typeof(value) temp = value;                                                \
    array = _vectorPush(array, &temp);                                         \
  }
#define vectorPop(array, value_ptr) _vectorPop(array, value_ptr)

#define vectorCapacity(array) _vectorFieldGet(array, VECTOR_CAPACITY)
#define vectorLength(array) _vectorFieldGet(array, VECTOR_LENGTH)
#define vectorStride(array) _vectorFieldGet(array, VECTOR_STRIDE)

#define vectorReserve(type, capacity) _vectorCreate(capacity, sizeof(type))
#define vectorClear(array) _vectorFieldSet(array, VECTOR_LENGTH, 0)