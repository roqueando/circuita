#pragma once

#include <stdlib.h>


typedef struct {
  double value;
} resistor_t;

typedef struct {
  resistor_t* resistors;
  size_t length;
  size_t capacity;
} resistor_array;

resistor_t in_series(resistor_array rs);
resistor_t in_parallel(resistor_array rs);

void resistor_array_init(resistor_array *arr, size_t initial_capacity);
void resistor_array_push(resistor_array *arr, resistor_t r);
void resistor_array_free(resistor_array *arr);
