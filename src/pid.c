#include "pid.h"
#include <stdio.h>

double measurement_output, signal_control;
double reference = 0.0;
double Kp = 0.0;  // Ganho Proporcional
double Ki = 0.0;  // Ganho Integral
double Kd = 0.0;  // Ganho Derivativo
int T = 1.0;      // Período de Amostragem (ms)
unsigned long last_time;
double total_error, previous_error = 0.0;
int signal_control_MAX = 100.0;
int signal_control_MIN = -100.0;

void pid_configure_constants(double Kp_, double Ki_, double Kd_) {
	Kp = Kp_;
	Ki = Ki_;
	Kd = Kd_;
}

void pid_update_reference(float reference_) {
	reference = (double) reference_;
}

double pid_control(double measurement_output) {
	double error = reference - measurement_output;

	total_error += error; // Acumula o erro (Termo Integral)

	if (total_error >= signal_control_MAX) {
		total_error = signal_control_MAX;
	} else if (total_error <= signal_control_MIN) {
		total_error = signal_control_MIN;
	}

	double delta_error = error - previous_error; // Diferença entre os erros (Termo Derivativo)

	signal_control = Kp*error + (Ki*T)*total_error + (Kd/T)*delta_error; // PID calcula sinal de controle

	if (signal_control >= signal_control_MAX) {
		signal_control = signal_control_MAX;
	} else if (signal_control <= signal_control_MIN) {
		signal_control = signal_control_MIN;
	}

	previous_error = error;

	return signal_control;
}
