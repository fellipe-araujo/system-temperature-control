#include <stdio.h>
#include "uart.h"
#include "led_16x2.h"
#include "bme280_get_temperature.h"

int main(int argc, char *argv[]) {
  int uart = init_uart();

  write_uart_message(uart, 0);
  read_uart_message(uart, 0);

  close(uart);

  lcd_print(30.5, 28.7, 34.0);

  bme280_get_temperature();

  return 0;
}
