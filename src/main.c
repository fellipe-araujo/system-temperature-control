#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "uart.h"
#include "led_16x2.h"
#include "bme280_get_temperature.h"
#include "pid.h"
#include "gpio.h"
#include "on_off.h"

void shut_down_system();

int uart;

int main(int argc, char *argv[]) {
  float TR = 0.0, TI = 0.0, TE = 0.0, control_output = 0.0;
  float Kp = 5.0, Ki = 1.0, Kd = 5.0;
  float TR_temp;

  uart = init_uart();
  setup_gpio();
  setup_bme280();

  signal(SIGINT, shut_down_system);

  // pid_configure_constants(Kp, Ki, Kd);

  while (1) {
    // TR_temp = potentiometer_temperature(uart, TR);

    // if (TR_temp > TE && TR_temp < 100) {
    //   TR = TR_temp;
    // }
    TR = 40.0;

    TI = DS18B20_temperature(uart, TI);
    TE = bme280_get_temperature();

    lcd_print(TR, TI, TE);

    // pid_update_reference(TR);

    // control_output = pid_control(TI);

    on_off_control(TR, TI, 2);

    // manage_gpio_devices(control_output);

    sleep(1);
  }

  return 0;
}

void shut_down_system() {
  disable_fan_and_resistor();
  close_uart(uart);
  close_bme280();
  printf("======== Finished System! ========");
  exit(0);
}
