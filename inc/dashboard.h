#ifndef DASHBOARD_H_
#define DASHBOARD_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include "main.h"

#define WIDTH 72
#define HEIGHT 15

void info(float TR, float TI, float TE, int potentiometer, int pid, float Kp, float Ki, float Kd, int hysteresis, int use_key_switch);
void menu();

#endif
