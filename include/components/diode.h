#ifndef DIODE_H
#define DIODE_H

typedef enum {
  DIRECTLY = 0,
  REVERSED,
  DIRECTION_COUNT
} direction_t;

typedef struct {
  direction_t polarization;
  double tension;
} diode_t;

double get_tension_over_diode(diode_t d, double vcc);

#endif // DIODE_H
