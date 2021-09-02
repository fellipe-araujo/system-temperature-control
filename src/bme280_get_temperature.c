#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "bme280.h"

struct identifier {
  uint8_t dev_addr;
  int8_t fd;
};

void user_delay_us(uint32_t period, void *intf_ptr);

void print_sensor_data(struct bme280_data *device_data);

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);

int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev);

void bme280_get_temperature() {
  struct bme280_dev dev;
  struct identifier id;
  struct bme280_data device_data;

  /* Variable to define the result */
  int8_t rslt = BME280_OK;

  if ((id.fd = open("/dev/i2c-1", O_RDWR)) < 0) {
    fprintf(stderr, "Failed to open the i2c bus %s\n", "/dev/i2c-1");
    exit(1);
  }

  /* Make sure to select BME280_I2C_ADDR_PRIM or BME280_I2C_ADDR_SEC as needed */
  id.dev_addr = BME280_I2C_ADDR_PRIM;

  dev.intf = BME280_I2C_INTF;
  dev.read = user_i2c_read;
  dev.write = user_i2c_write;
  dev.delay_us = user_delay_us;

  /* Update interface pointer with the structure that contains both device address and file descriptor */
  dev.intf_ptr = &id;

  if (ioctl(id.fd, I2C_SLAVE, id.dev_addr) < 0) {
    fprintf(stderr, "Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }

  /* Initialize the bme280 */
  rslt = bme280_init(&dev);

  if (rslt != BME280_OK) {
    fprintf(stderr, "Failed to initialize the device (code %+d).\n", rslt);
    exit(1);
  }

  rslt = stream_sensor_data_forced_mode(&dev);
  rslt = bme280_get_sensor_data(BME280_ALL, &device_data, &dev);

  if (rslt != BME280_OK) {
    fprintf(stderr, "Failed to get sensor data (code %+d).", rslt);
  }

  print_sensor_data(&device_data);

  if (rslt != BME280_OK) {
    fprintf(stderr, "Failed to stream sensor data (code %+d).\n", rslt);
    exit(1);
  }
}

void user_delay_us(uint32_t period, void *intf_ptr) {
  usleep(period * 1000);
}

void print_sensor_data(struct bme280_data *device_data) {
  float temp, press, hum;

#ifdef BME280_FLOAT_ENABLE
  temp = device_data->temperature;
  press = 0.01 * device_data->pressure;
  hum = device_data->humidity;
#else
#ifdef BME280_64BIT_ENABLE
  temp = 0.01f * device_data->temperature;
  press = 0.0001f * device_data->pressure;
  hum = 1.0f / 1024.0f * device_data->humidity;
#else
  temp = 0.01f * device_data->temperature;
  press = 0.01f * device_data->pressure;
  hum = 1.0f / 1024.0f * device_data->humidity;
#endif
#endif
  printf("%0.2lf deg C, %0.2lf hPa, %0.2lf%%\n", temp, press, hum);
}

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr) {
  struct identifier id;

  id = *((struct identifier *)intf_ptr);

  write(id.fd, &reg_addr, 1);
  read(id.fd, data, len);

  return 0;
}

int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr) {
  uint8_t *buf;
  struct identifier id;

  id = *((struct identifier *)intf_ptr);

  buf = malloc(len + 1);
  buf[0] = reg_addr;
  memcpy(buf + 1, data, len);

  if (write(id.fd, buf, len + 1) < (uint16_t)len) {
    return BME280_E_COMM_FAIL;
  }

  free(buf);

  return BME280_OK;
}

int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev) {
  /* Variable to define the result */
  int8_t rslt = BME280_OK;

  /* Variable to define the selecting sensors */
  uint8_t settings_sel = 0;

  /* Variable to store minimum wait time between consecutive measurement in force mode */
  uint32_t req_delay;

  /* Structure to get the pressure, temperature and humidity values */
  struct bme280_data device_data;

  /* Recommended mode of operation: Indoor navigation */
  dev->settings.osr_h = BME280_OVERSAMPLING_1X;
  dev->settings.osr_p = BME280_OVERSAMPLING_16X;
  dev->settings.osr_t = BME280_OVERSAMPLING_2X;
  dev->settings.filter = BME280_FILTER_COEFF_16;

  settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

  /* Set the sensor settings */
  rslt = bme280_set_sensor_settings(settings_sel, dev);

  if (rslt != BME280_OK) {
    fprintf(stderr, "Failed to set sensor settings (code %+d).", rslt);

    return rslt;
  }

  printf("Temperature, Pressure, Humidity\n");

  /*Calculate the minimum delay required between consecutive measurement based upon the sensor enabled
    *  and the oversampling configuration. */
  req_delay = bme280_cal_meas_delay(&dev->settings);

  /* Continuously stream sensor data */
  while (1) {
    /* Set the sensor to forced mode */
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
    
    if (rslt != BME280_OK) {
      fprintf(stderr, "Failed to set sensor mode (code %+d).", rslt);
      break;
    }

    /* Wait for the measurement to complete and print data */
    dev->delay_us(req_delay, dev->intf_ptr);
    rslt = bme280_get_sensor_data(BME280_ALL, &device_data, dev);

    if (rslt != BME280_OK) {
      fprintf(stderr, "Failed to get sensor data (code %+d).", rslt);
      break;
    }

    print_sensor_data(&device_data);
  }

  return rslt;
}
