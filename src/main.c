#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <core/resistor.h>
//#include <libconfig.h>

int main(void)
{
  resistor_array ra;
  resistor_array_init(&ra, 2);

  resistor_t r1 = {5, KOHM};
  resistor_t r2 = {10, KOHM};
  resistor_t r3 = {5, KOHM};

  resistor_array_push(&ra, r1);
  resistor_array_push(&ra, r2);
  resistor_array_push(&ra, r3);

  resistor_t result = in_series(ra);
  char *unit = "";
  int32_t n = 0;


  switch (result.measure) {
  case OHM:
    unit = "";
    break;
  case KOHM:
    unit = "k";
    break;
  case MOHM:
    unit = "M";
    break;
  }

  if (result.value % MOD_KOHM == 0) {
    n = result.value / MOD_KOHM;
  } else if (result.value % MOD_MOHM == 0)  {
    n = result.value / MOD_MOHM;
  }

  printf("resultado: %d%s", n, unit);

  resistor_array_free(&ra);
  /*
  config_t cfg;
  config_setting_t *setting;
  const char *str;

  config_init(&cfg);

  if (!config_read_file(&cfg, "components.cfg")) {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return EXIT_FAILURE;
  }

  const char* v_value;
  if (config_lookup_string(&cfg, "inputs.V", &v_value)) {
    printf("V: %s\n", v_value);
  }

  config_destroy(&cfg);
  */
  return 0;
}
