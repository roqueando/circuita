#ifndef UTIL_H
#define UTIL_H

typedef enum {
  MICRO = 0,
  MILI,
  NORM,
  KILO,
  MEGA,
  MEASURE_COUNT
} measure_t;

typedef struct {
  double value;
  measure_t measure;
} measure_value_t;

measure_value_t get_measure(double value);
const char* get_measure_name(measure_t m);

#endif // UTIL_H
