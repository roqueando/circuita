#include <core/resistor.h>
#include <stdio.h>
#include <math.h>

bool is_diff(double x, double y) {
  return round(x) != round(y);
}

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

  if (is_diff((result.value / MOD_KOHM), expected.value)) {
    fprintf(stderr, "FAILED: r1.value[%f], r2.value[%f], result.value[%f], expected.value[%f]\n",
            r1.value, r2.value, result.value, expected.value);
    return 0;
  }

  resistor_array_free(&ra);
  return 1;
}

int test_in_series_resistor_should_operate_in_kohm_to_mohm()
{
  resistor_t r1 = {1, MOHM};
  resistor_t r2 = {3, KOHM};
  resistor_t expected = {1003, KOHM};
  
  resistor_array ra;
  resistor_array_init(&ra, 2);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);

  resistor_t result = in_series(ra);
  auto kohm_result = to_kohm(result);

  if (is_diff(kohm_result.value, expected.value)) {
    fprintf(stderr, "FAILED: r1.value[%f], r2.value[%f], result.value[%f], expected.value[%f]\n",
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
  resistor_t expected = {3.33, KOHM};
  
  resistor_array ra;
  resistor_array_init(&ra, 2);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);

  resistor_t result = in_parallel(ra);
  double mod_result = round(result.value / MOD_KOHM);

  if (round(result.value / MOD_KOHM) != round((double)expected.value)) {
    fprintf(stderr, "FAILED: r1.value[%.2f], r2.value[%.2f], result.value[%.2f], expected.value[%.2f]\n",
            r1.value, r2.value, (result.value / MOD_KOHM), expected.value);
    return 0;
  }


  resistor_array_free(&ra);
  return 1;
}

int test_in_parallel_resistor_should_operate_from_kohm_to_mohm()
{
  resistor_t r1 = {1, MOHM};
  resistor_t r2 = {3, KOHM};
  resistor_t expected = {2.9, KOHM};
  
  resistor_array ra;
  resistor_array_init(&ra, 2);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);

  resistor_t result = in_parallel(ra);
  auto kohm_result = to_kohm(result);

  if (is_diff(kohm_result.value, expected.value)) {
    fprintf(stderr, "FAILED: r1.value[%.2f], r2.value[%.2f], result.value[%.2f], expected.value[%.2f]\n",
            r1.value, r2.value, (result.value / MOD_KOHM), expected.value);
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

  if (!test_in_series_resistor_should_operate_in_kohm_to_mohm()) {
    return 1;
  }

  if (!test_in_parallel_resistor_should_operate_from_kohm_to_mohm()) {
    return 1;
  }

  return 0;
}
