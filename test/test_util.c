#include "components/util.h"
#include <stdio.h>

int test_should_get_value_in_kilo() {
  double value = 1500.0;
  double expected_value = 1.5;
  measure_t expected_measure = KILO;
  measure_value_t m = get_measure(value);

  if (!(m.measure == KILO && m.value == 1.5)) {
    fprintf(stderr,
            "%s FAILED: value[%.2f], measure[%s], "
            "expected.value[%.2f], expected.measure[%s]\n",
            __func__, m.value, get_measure_name(m.measure), expected_value,
            get_measure_name(expected_measure));
    return 0;
  }
  return 1;
}

int test_should_get_value_in_mega() {
  double value = 1000000.0;
  double expected_value = 1.0;
  measure_t expected_measure = MEGA;
  measure_value_t m = get_measure(value);

  if (!(m.measure == MEGA && m.value == 1.0)) {
    fprintf(stderr,
            "%s FAILED: value[%.2f], measure[%s], "
            "expected.value[%.2f], expected.measure[%s]\n",
            __func__, m.value, get_measure_name(m.measure), expected_value,
            get_measure_name(expected_measure));
    return 0;
  }
  return 1;
}

int test_should_get_value_in_mili() {
  double value = 0.016;
  double expected_value = 16.0;
  measure_t expected_measure = MILI;
  measure_value_t m = get_measure(value);

  if (!(m.measure == MILI && m.value == 16.0)) {
    fprintf(stderr,
            "%s FAILED: value[%.2f], measure[%s], "
            "expected.value[%.2f], expected.measure[%s]\n",
            __func__, m.value, get_measure_name(m.measure), expected_value,
            get_measure_name(expected_measure));
    return 0;
  }
  return 1;
}


int main(void) {
  if (!test_should_get_value_in_kilo()) {
    return 1;
  }

  if (!test_should_get_value_in_mega()) {
    return 1;
  }

  if (!test_should_get_value_in_mili()) {
    return 1;
  }

  printf("==== TESTS PASSED ====\n");

  return 0;
}
