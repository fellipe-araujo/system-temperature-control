#include "on_off.h"

int on_off_control(float TR, float TI, int hysteresis) {
  int control_output = 0;

  float top_limit = TR + hysteresis / 2.0;
  float bottom_limit = TR - hysteresis / 2.0;

  if (TI >= top_limit) {
    control_output = -100;
    manage_gpio_devices(control_output);
  } else if (TI <= bottom_limit) {
    control_output = 100;
    manage_gpio_devices(control_output);
  }

  return control_output;
}
