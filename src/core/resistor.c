#include <core/resistor.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

void resistor_array_init(resistor_array *arr, size_t initial_capacity) {
    arr->length = 0;
    arr->capacity = (initial_capacity > 0 ? initial_capacity : 1);
    arr->resistors = malloc(arr->capacity * sizeof *arr->resistors);
    if (!arr->resistors) {
        exit(EXIT_FAILURE);
    }
}

void resistor_array_push(resistor_array *arr, resistor_t r) {
    if (arr->length == arr->capacity) {
        size_t newcap = arr->capacity * 2;
        resistor_t *tmp = realloc(arr->resistors, newcap * sizeof *tmp);
        if (!tmp) {
            // o arr->data continua válido
            exit(EXIT_FAILURE);
        }
        arr->resistors = tmp;
        arr->capacity = newcap;
    }
    arr->resistors[arr->length++] = r;
}

// liberação do array (não libera strings internas caso existam)
void resistor_array_free(resistor_array *arr) {
    free(arr->resistors);
    arr->resistors = NULL;
    arr->length = arr->capacity = 0;
}

resistor_t in_series(resistor_array rs)
{
  double result = 0.0;
  resistor_t r_result;
  for (size_t i = 0; i < rs.length; ++i) {
    switch(rs.resistors[i].measure) {
    case OHM:
      result += rs.resistors[i].value;
      break;
    case KOHM:
      result += rs.resistors[i].value * MOD_KOHM;
      break;
    case MOHM:
      result += rs.resistors[i].value * MOD_MOHM;
      break;
    }
  }

  r_result.value = result;

  if (fmod(result, MOD_KOHM) == 0.0) {
    r_result.measure = KOHM;
  } else if (fmod(result, MOD_MOHM) == 0.0) {
    r_result.measure = MOHM;
  }

  return r_result;
}

resistor_t in_parallel(resistor_array rs)
{
  double result = 1.0;
  resistor_t r_result;
  resistor_t in_series_result = in_series(rs);

  for (size_t i = 0; i < rs.length; ++i) {
    switch(rs.resistors[i].measure) {
    case OHM:
      result *= rs.resistors[i].value;
      break;
    case KOHM:
      result *= rs.resistors[i].value * MOD_KOHM;
      break;
    case MOHM:
      result *= rs.resistors[i].value * MOD_MOHM;
      break;
    }
  }

  r_result.value = result / in_series_result.value;

  if (fmod(result, MOD_KOHM) == 0.0) {
    r_result.measure = KOHM;
  } else if (fmod(result, MOD_MOHM) == 0.0) {
    r_result.measure = MOHM;
  }

  return r_result;
}

resistor_t to_kohm(resistor_t r)
{
  resistor_t result;
  result.measure = KOHM;

  result.value = r.value / MOD_KOHM;
  return result;
}
