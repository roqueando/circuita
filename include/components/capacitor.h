#ifndef CAPACITOR_H
#define CAPACITOR_H

#include <stdlib.h>

typedef struct {
  double value;
} capacitor_t;

typedef struct {
  capacitor_t* capacitors;
  size_t length;
  size_t capacity;
} capacitor_array;

capacitor_t in_series(capacitor_array cs);
capacitor_t in_parallel(capacitor_array cs);

void capacitor_array_init(capacitor_array *arr, size_t initial_capacity);
void capacitor_array_push(capacitor_array *arr, capacitor_t r);
void capacitor_array_free(capacitor_array *arr);

#endif // CAPACITOR_H
