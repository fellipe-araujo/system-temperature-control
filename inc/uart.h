#define SERVER_CODE 0x01
#define REQUEST_CODE 0x23
#define SEND_CODE 0x16
#define REQUEST_DS18B20_TEMPERATURE 0xC1
#define REQUEST_POTENTIOMETER_TEMPERATURE 0xC2
#define REQUEST_KEY_STATE 0xC3
#define SEND_CONTROL_SIGNAL 0xD1
#define DS18B20 0
#define POTENTIOMETER 1
#define KEY_STATE 2

#ifndef UART_H_
#define UART_H_

int init_uart();
void write_uart_message(int uart, int code);
float read_uart_message(int uart, int code);
float potentiometer_temperature(int uart, float TR);
float DS18B20_temperature(int uart, float TI);
void close_uart(int uart);

#endif
