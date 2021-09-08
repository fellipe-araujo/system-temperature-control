#ifndef BME280_TEMPERATURE_H
#define BME280_TEMPERATURE_H

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "bme280.h"

void setup_bme280();
float bme280_get_temperature();
void close_bme280();

#endif
