#include "dynamic_array.h"
#include "../core/asserts.h"
#include "../core/orb_memory.h"
#include <strings.h>

orb_dynamic_array _orb_dynamic_array_create(usize capacity, usize stride) {
  orb_dynamic_array array = {
      .length = 0,
      .capacity = capacity,
      .stride = stride,
      .items = orb_allocate(capacity * stride, MEMORY_TAG_DYNAMIC_ARRAY),
  };

  return array;
}

void orb_dynamic_array_destroy(orb_dynamic_array *array) {
  orb_free(array->items, array->capacity, MEMORY_TAG_DYNAMIC_ARRAY);
}

void _orb_dynamic_array_resize(orb_dynamic_array *array) {
  usize new_capacity = ORB_DYNAMIC_ARRAY_RESIZE_FACTOR * array->capacity;
  void *new_items =
      orb_allocate(new_capacity * array->stride, MEMORY_TAG_DYNAMIC_ARRAY);

  orb_memory_copy(new_items, array->items, array->capacity * array->stride);

  orb_free(array->items, array->capacity, MEMORY_TAG_DYNAMIC_ARRAY);

  array->items = new_items;
  array->capacity = new_capacity;
}

void _orb_dynamic_array_push(orb_dynamic_array *array, const void *value_ptr) {
  if (array->length >= array->capacity) {
    _orb_dynamic_array_resize(array);
  }

  usize address = (usize)array->items;
  address += array->length * array->stride;

  orb_memory_copy((void *)address, value_ptr, array->stride);
  array->length += 1;
}

void orb_dynamic_array_pop(orb_dynamic_array *array, void *destination) {
  usize address = (usize)array->items;
  address += ((array->length - 1) * array->stride);

  orb_memory_copy(destination, (void *)address, array->stride);

  array->length -= 1;
}

void _orb_dynamic_array_insert_at(orb_dynamic_array *array, usize index,
                                  const void *value_ptr) {
  ORB_DEBUG_ASSERT(index < array->length,
                   "insert_at failed: index out of bounds");

  if (array->length >= array->capacity) {
    _orb_dynamic_array_resize(array);
  }

  usize insert_address = (usize)array->items;
  insert_address += (index * array->stride);

  if (index != array->length - 1) {
    orb_memory_copy((void *)(insert_address + array->stride),
                    (void *)insert_address,
                    array->stride * (array->length - index));
  }

  orb_memory_copy((void *)insert_address, value_ptr, array->stride);

  array->length += 1;
}

void orb_dynamic_array_remove_at(orb_dynamic_array *array, usize index,
                                 void *destination) {
  ORB_DEBUG_ASSERT(index < array->length,
                   "remove_at failed: index out of bounds");

  usize remove_address = (usize)array->items;
  remove_address += (index * array->stride);

  orb_memory_copy(destination, (void *)remove_address, array->stride);

  // copies elements after removed element one element downwards
  if (index != array->length - 1) {
    orb_memory_copy((void *)remove_address,
                    (void *)(remove_address + index * array->stride),
                    array->stride * (array->length - index));
  }

  array->length -= 1;
}
