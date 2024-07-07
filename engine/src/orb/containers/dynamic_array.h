#pragma once

#include "../core/types.h"

typedef struct orb_dynamic_array {
  usize capacity;
  usize length;
  usize stride;
  void *items;
} orb_dynamic_array;

ORB_API orb_dynamic_array _orb_dynamic_array_create(usize capacity,
                                                    usize stride);

#define orb_dynamic_array_create(type)                                         \
  _orb_dynamic_array_create(ORB_DYNAMIC_ARRAY_DEFAULT_CAPACITY, sizeof(type))

#define orb_dynamic_array_create_with_size(type, capacity)                     \
  _orb_dynamic_array_create(capacity, sizeof(type))

ORB_API void orb_dynamic_array_destroy(orb_dynamic_array *array);

ORB_API void _orb_dynamic_array_resize(orb_dynamic_array *array);

ORB_API void _orb_dynamic_array_push(orb_dynamic_array *array,
                                     const void *value_ptr);

#define orb_dynamic_array_push(array, value)                                   \
  {                                                                            \
    auto temp = value;                                                         \
    _orb_dynamic_array_push(&array, &value);                                   \
  }

ORB_API void orb_dynamic_array_pop(orb_dynamic_array *array, void *destination);

ORB_API void _orb_dynamic_array_insert_at(orb_dynamic_array *array, usize index,
                                          const void *value_ptr);

#define orb_dynamic_array_insert_at(array, index, value)                       \
  {                                                                            \
    auto temp = value;                                                         \
    _orb_dynamic_array_insert_at(&array, index, &value);                       \
  }

ORB_API void orb_dynamic_array_remove_at(orb_dynamic_array *array, usize index,
                                         void *destination);

#define orb_dynamic_array_clear(array) array.length = 0

#define ORB_DYNAMIC_ARRAY_DEFAULT_CAPACITY 8
#define ORB_DYNAMIC_ARRAY_RESIZE_FACTOR 2
