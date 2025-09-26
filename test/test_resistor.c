#include "components/resistor.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

bool is_diff(double x, double y) { return round(x) != round(y); }

int test_in_series_resistor() {
  resistor_t r1 = {5000};
  resistor_t r2 = {10000};
  resistor_t expected = {15000};

  resistor_array ra;
  resistor_array_init(&ra, 2);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);

  resistor_t result = in_series(ra);

  if (is_diff(result.value, expected.value)) {
    fprintf(stderr,
            "%s FAILED: r1.value[%f], r2.value[%f], result.value[%f], "
            "expected.value[%f]\n",
            __func__, r1.value, r2.value, result.value, expected.value);
    return 0;
  }

  resistor_array_free(&ra);
  return 1;
}

int test_in_series_resistor_should_operate_in_kohm_to_mohm() {
  resistor_t r1 = {1000000};
  resistor_t r2 = {3000};
  resistor_t expected = {1003000};

  resistor_array ra;
  resistor_array_init(&ra, 2);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);

  resistor_t result = in_series(ra);

  if (is_diff(result.value, expected.value)) {
    fprintf(stderr,
            "%s FAILED: r1.value[%f], r2.value[%f], result.value[%f], "
            "expected.value[%f]\n",
            __func__, r1.value, r2.value, result.value, expected.value);
    return 0;
  }

  resistor_array_free(&ra);
  return 1;
}

int test_in_parallel_resistor() {
  resistor_t r1 = {5000};
  resistor_t r2 = {10000};
  resistor_t expected = {3333.33};

  resistor_array ra;
  resistor_array_init(&ra, 2);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);

  resistor_t result = in_parallel(ra);

  if (round(result.value) != round((double)expected.value)) {
    fprintf(stderr,
            "%s FAILED: r1.value[%.2f], r2.value[%.2f], result.value[%.2f], "
            "expected.value[%.2f]\n",
            __func__, r1.value, r2.value, result.value, expected.value);
    return 0;
  }

  resistor_array_free(&ra);
  return 1;
}

int test_in_parallel_resistor_with_three_resistors() {
  resistor_t r1 = {5000};
  resistor_t r2 = {10000};
  resistor_t r3 = {10000};
  resistor_t expected = {2500};

  resistor_array ra;
  resistor_array_init(&ra, 3);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);
  resistor_array_push(&ra, r3);

  resistor_t result = in_parallel(ra);

  if (round(result.value) != round((double)expected.value)) {
    fprintf(stderr,
            "%s FAILED: r1.value[%.2f], r2.value[%.2f], result.value[%.2f], "
            "expected.value[%.2f]\n",
            __func__, r1.value, r2.value, result.value, expected.value);
    return 0;
  }

  resistor_array_free(&ra);
  return 1;
}

int test_in_parallel_resistor_should_operate_from_kohm_to_mohm() {
  resistor_t r1 = {1000000};
  resistor_t r2 = {3000};
  resistor_t expected = {2991.0};

  resistor_array ra;
  resistor_array_init(&ra, 2);
  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);

  resistor_t result = in_parallel(ra);

  if (is_diff(round(result.value), round(expected.value))) {
    fprintf(stderr,
            "%s FAILED: r1.value[%.2f], r2.value[%.2f], result.value[%.2f], "
            "expected.value[%.2f]\n",
            __func__, r1.value, r2.value, round(result.value), expected.value);
    return 0;
  }

  resistor_array_free(&ra);
  return 1;
}

int main(void) {
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
  if (!test_in_parallel_resistor_with_three_resistors()) {
    return 1;
  }

  printf("==== [test_resistor] TESTS PASSED ====\n");

  return 0;
}
