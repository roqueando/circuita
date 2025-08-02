#pragma once

#include <stdlib.h>

#define MOD_KOHM 1000
#define MOD_MOHM 1000000

typedef enum {
  OHM,
  KOHM,
  MOHM,
} r_measure;

typedef struct {
  double value;
  r_measure measure;
} resistor_t;

typedef struct {
  resistor_t* resistors;
  size_t length;
  size_t capacity;
} resistor_array;

resistor_t in_series(resistor_array rs);
resistor_t in_parallel(resistor_array rs);
resistor_t to_kohm(resistor_t r);


void resistor_array_init(resistor_array *arr, size_t initial_capacity);
void resistor_array_push(resistor_array *arr, resistor_t r);
void resistor_array_free(resistor_array *arr);
