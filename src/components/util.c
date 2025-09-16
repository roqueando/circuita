#include "components/util.h"

#include <math.h>
#include <stdlib.h>

measure_value_t get_measure(double value)
{
  size_t index = 2;
  double v = value;
  measure_value_t m = {0};

  while(v > 0 && fabs(v) < 1.0 && index > 0) {
    v *= 1000.0;
    index--;
  }

  while(fabs(v) >= 1000.0 && index < 5) {
    v /= 1000.0;
    index++;
  }
  m.value = v;
  m.measure = index;

  return m;
}

const char* get_measure_name(measure_t m)
{
  switch(m) {
  case MICRO:
    return "u";
  case MILI:
    return "m";
  case NORM:
    return "";
  case KILO:
    return "k";
  case MEGA:
    return "M";
  default:
    return "und";
  }
}
