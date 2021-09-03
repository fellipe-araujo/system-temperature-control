#include <stdio.h>
#include <string.h>
#include <unistd.h>  // Used for UART
#include <fcntl.h>   // Used for UART
#include <termios.h> // Used for UART
#include "crc16.h"
#include "uart.h"

int init_uart() {
  int uart0_filestream = -1;

  uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY); // Open in non blocking read/write mode

  if (uart0_filestream == -1) {
    printf("Erro - Não foi possível iniciar a UART.\n");
  } else {
    printf("UART inicializada!\n");
  }

  struct termios options;
  tcgetattr(uart0_filestream, &options);
  options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; // Set baud rate
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(uart0_filestream, TCIFLUSH);
  tcsetattr(uart0_filestream, TCSANOW, &options);

  return uart0_filestream;
}

void write_uart_message(int uart, int code) {
  unsigned char tx_buffer[20];
  unsigned char *p_tx_buffer;
  p_tx_buffer = &tx_buffer[0];

  *p_tx_buffer++ = SERVER_CODE;
  *p_tx_buffer++ = REQUEST_CODE;

  if (code == DS18B20) {
    *p_tx_buffer++ = REQUEST_DS18B20_TEMPERATURE;
  } else if (code == POTENTIOMETER) {
    *p_tx_buffer++ = REQUEST_POTENTIOMETER_TEMPERATURE;
  } else if (code == KEY_STATE) {
    *p_tx_buffer++ = REQUEST_KEY_STATE;
  }

  *p_tx_buffer++ = 3;
  *p_tx_buffer++ = 7;
  *p_tx_buffer++ = 2;
  *p_tx_buffer++ = 3;

  short crc = calcula_CRC(tx_buffer, (p_tx_buffer - &tx_buffer[0]));

  memcpy(&tx_buffer[(p_tx_buffer - &tx_buffer[0])], &crc, 2);

  p_tx_buffer += 2;

  if (uart != -1) {
    printf("Escrevendo caracteres na UART ...\n");

    int count = write(uart, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));

    if (count < 0) {
      printf("UART TX error!\n");
    } else {
      printf("Escrito!\n");
    }
  }

  sleep(1);
}

float read_uart_message(int uart, int code) {
  float response_float;
  int response_integer;
  //-------------- CHECK FOR ANY RX BYTES --------------
  if (uart != -1) {
    // Read up to 255 characters from the port if they are there
    unsigned char rx_buffer[256];
    int rx_length = read(uart, (void *)rx_buffer, 255);

    if (rx_length < 0) {
      printf("Erro na leitura! RX_LENGTH: %d\n", rx_length);
      response_float = -1;
      response_integer = -1;
    } else if (rx_length == 0) {
      printf("Nenhum dado disponível.\n");
      response_float = -1;
      response_integer = -1;
    } else {
      rx_buffer[rx_length] = '\0';

      if (code == 0 || code == 1) {
        float float_data;
        memcpy(&float_data, &rx_buffer[3], 4);
        // printf("Float: %f\n", float_data);
        response_float = float_data;
      } else {
        int integer_data;
        memcpy(&integer_data, &rx_buffer[3], 4);
        // printf("Integer: %d\n", integer_data);
        response_integer = integer_data;
      }
    }
  }

  if (code == 0 || code == 1) {
    return response_float;
  } else {
    return response_integer;
  }
}
