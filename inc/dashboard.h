#ifndef DASHBOARD_H_
#define DASHBOARD_H_

#define WIDTH 72
#define HEIGHT 15

void info(float TR, float TI, float TE, int potentiometer, int pid, float Kp, float Ki, float Kd, int hysteresis, int use_key_switch);
void menu();

#endif
