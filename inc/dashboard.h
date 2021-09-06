#ifndef DASHBOARD_H_
#define DASHBOARD_H_

#define WIDTH 70
#define HEIGHT 15

void info(float TR, float TI, float TE, int potentiometer, int pid, float Kp, float Ki, float Kd, int hysteresis);
void menu(int potentiometer, int pid, float TR, float hysteresis, float Kp, float Ki, float Kd);

#endif
