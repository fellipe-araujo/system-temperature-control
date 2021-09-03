#include <stdio.h>
#include "gpio.h"

void on_off_control(float TR, float TI, int hysteresis) {
  float control_output = 0.0;

  float top_limit = TR + hysteresis / 2.0;
  float bottom_limit = TR - hysteresis / 2.0;

  if (TI >= top_limit) {
    control_output = -100;
    manage_gpio_devices(control_output);
  } else if (TI <= bottom_limit) {
    control_output = 100;
    manage_gpio_devices(control_output);
  }
}
