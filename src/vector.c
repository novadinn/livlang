#include "vector.h"

#include "logger.h"

#include <stdlib.h>
#include <string.h>

void *_vectorCreate(u64 length, u64 stride) {
  u64 header_size = VECTOR_FIELD_LENGTH * sizeof(u64);
  u64 array_size = length * stride;
  u64 *new_array = malloc(header_size + array_size);
  memset(new_array, 0, header_size + array_size);

  new_array[VECTOR_CAPACITY] = length;
  new_array[VECTOR_LENGTH] = 0;
  new_array[VECTOR_STRIDE] = stride;

  return (void *)(new_array + VECTOR_FIELD_LENGTH);
}

void _vectorDestroy(void *array) {
  u64 *header = (u64 *)array - VECTOR_FIELD_LENGTH;
  free(header);
}

u64 _vectorFieldGet(void *array, u64 field) {
  u64 *header = (u64 *)array - VECTOR_FIELD_LENGTH;

  return header[field];
}

void _vectorFieldSet(void *array, u64 field, u64 value) {
  u64 *header = (u64 *)array - VECTOR_FIELD_LENGTH;
  header[field] = value;
}

void *_vectorResize(void *array) {
  u64 length = vectorLength(array);
  u64 stride = vectorStride(array);
  u64 capacity = vectorCapacity(array);

  void *temp = _vectorCreate((VECTOR_RESIZE_FACTOR * capacity), stride);
  memcpy(temp, array, length * stride);

  _vectorFieldSet(temp, VECTOR_LENGTH, length);
  _vectorDestroy(array);

  return temp;
}

void *_vectorPush(void *array, const void *value_ptr) {
  u64 length = vectorLength(array);
  u64 stride = vectorStride(array);
  u64 capacity = vectorCapacity(array);
  if (length >= capacity) {
    array = _vectorResize(array);
  }

  u64 addr = (u64)array;
  addr += (length * stride);
  memcpy((void *)addr, value_ptr, stride);
  _vectorFieldSet(array, VECTOR_LENGTH, length + 1);

  return array;
}

void _vectorPop(void *array, void *dest) {
  u64 length = vectorLength(array);
  u64 stride = vectorStride(array);

  u64 addr = (u64)array;
  addr += ((length - 1) * stride);
  memcpy(dest, (void *)addr, stride);
  _vectorFieldSet(array, VECTOR_LENGTH, length - 1);
}
