#include "main.h"

void info_thread();
void write_csv_thread();
void get_temperatures();
void check_key_state();
void get_control_signal();
void shut_down_system();

int uart;
float TR = 0.0, TI = 0.0, TE = 0.0, control_output = 0.0;
float Kp = 5.0, Ki = 1.0, Kd = 5.0;
int potentiometer = 1;
int pid = 1;
float hysteresis = 2.0;
int use_key_switch = 0;

int main(int argc, char *argv[]) {
  pthread_t tid[3];

  uart = init_uart();
  setup_gpio();
  setup_bme280();
  setup_lcd();
  setup_csv();
  
  pthread_create(&tid[0], NULL, (void *)menu, (void *)NULL);
	pthread_create(&tid[1], NULL, (void *)info_thread, (void *)NULL);
	pthread_create(&tid[2], NULL, (void *)write_csv_thread, (void *)NULL);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);

  return 0;
}

void info_thread() {
  signal(SIGINT, shut_down_system);

  while (1) {
    if (use_key_switch) {
      check_key_state();
    }

    get_temperatures();
    lcd_print(TR, TI, TE);

    get_control_signal();
    send_control_signal(uart, control_output);

    info(TR, TI, TE, potentiometer, pid, Kp, Ki, Kd, hysteresis, use_key_switch);

    usleep(700000);
  }
}

void write_csv_thread() {
  while (1) {
    if (pid) {
      write_csv(TR, TI, TE, control_output);
    } else if (!pid && control_output != 0) {
      write_csv(TR, TI, TE, control_output);
    }

    sleep(2);
  }
}

void get_temperatures() {
  if (potentiometer) {
    float TR_temp;
    TR_temp = potentiometer_temperature(uart, TR);

    if (TR_temp > TE && TR_temp < 100) {
      TR = TR_temp;
    }
  }
  
  TI = DS18B20_temperature(uart, TI);
  TE = bme280_get_temperature();
}

void set_use_key_switch() {
  if (use_key_switch) {
    use_key_switch = 0;
  } else {
    use_key_switch = 1;
  }
}

void check_key_state() {
  int key_state = get_key_state(uart);

  if (key_state == 0) {
    pid = 0;
  } else {
    pid = 1;
  }
}

void get_control_signal() {
  if (pid) {
    pid_configure_constants(Kp, Ki, Kd);
    pid_update_reference(TR);
    control_output = pid_control(TI);
    manage_gpio_devices(control_output);
  } else {
    int control_output_temp = on_off_control(TR, TI, hysteresis);
    
    if (control_output_temp != 0) {
      control_output = control_output_temp;
    }
  }
}

void shut_down_system() {
  disable_fan_and_resistor();
  ClrLcd();
  close_uart(uart);
  close_bme280();
  refresh();
  endwin();
  exit(0);
}

void set_potentiometer(int value) {
  potentiometer = value;
}

void set_pid(int value) {
  pid = value;
}

void set_temperature_reference_input(int potentiometer_active, float new_temperature) {
  if (!potentiometer_active && new_temperature > TE && new_temperature < 100) {
    get_control_signal();
    TR = new_temperature;
  }
}

void set_new_hysteresis(float new_hysteresis) {
  hysteresis = new_hysteresis;
}

void set_new_pid_constants(float new_Kp, float new_Ki, float new_Kd) {
  Kp = new_Kp;
  Ki = new_Ki;
  Kd = new_Kd;
}
