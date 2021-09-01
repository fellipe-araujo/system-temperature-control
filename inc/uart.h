#ifndef UART_H_
#define UART_H_

int init_uart();
void write_uart_message(int uart, int code);
void read_uart_message(int uart, int code);

#endif
