#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <core/resistor.h>

int main(void)
{
  printf("Hello there!\n");
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
