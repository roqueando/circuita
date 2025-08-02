#include <core/resistor.h>
#include <stdio.h>

int test_in_series_resistor()
{
  resistor_t r1 = {5, KOHM};
  resistor_t r2 = {10, KOHM};
  resistor_t expected = {15, KOHM};
  
  resistor_array ra;
  resistor_array_init(&ra, 2);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);

  resistor_t result = in_series(ra);

  if ((result.value / MOD_KOHM) != expected.value) {
    fprintf(stderr, "FAILED: r1.value[%d], r2.value[%d], result.value[%d], expected.value[%d]\n",
            r1.value, r2.value, result.value, expected.value);
    return 0;
  }

  resistor_array_free(&ra);
  return 1;
}

int test_in_parallel_resistor()
{
  resistor_t r1 = {5, KOHM};
  resistor_t r2 = {10, KOHM};
  resistor_t expected = {3, KOHM};
  
  resistor_array ra;
  resistor_array_init(&ra, 2);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);

  resistor_t result = in_parallel(ra);

  if ((result.value / MOD_KOHM) != expected.value) {
    fprintf(stderr, "FAILED: r1.value[%d], r2.value[%d], result.value[%d], expected.value[%d]\n",
            r1.value, r2.value, result.value, expected.value);
    return 0;
  }

  resistor_array_free(&ra);
  return 1;
}

int main(void)
{
  if (!test_in_series_resistor()) {
    return 1;
  }

  if (!test_in_parallel_resistor()) {
    return 1;
  }

  return 0;
}
