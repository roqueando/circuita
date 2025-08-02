#include <core/resistor.h>
#include <stdio.h>

int main(void)
{
  struct { resistor_t r1, r2, expected; } cases[] = {
    {5, KOHM},
    {10, KOHM},
    {15, KOHM}
  };
  resistor_array ra;
  resistor_array_init(&ra, 2);

  for (size_t i = 0; i < (int)(sizeof cases / sizeof *cases); ++i) {
    resistor_array_push(&ra, cases[i].r1);
    resistor_array_push(&ra, cases[i].r2);

    resistor_t result = in_series(ra);
    if (result.value != cases[i].expected.value) {
      fprintf(stderr, "FAILED: r1.value[%d], r2.value[%d], expected.value[%d]\n",
              cases[i].r1.value, cases[i].r2.value, cases[i].expected.value);
      return 1;
    }
  }


  resistor_array_free(&ra);
  return 0;
}
