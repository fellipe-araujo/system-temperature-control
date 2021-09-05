#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include "dashboard.h"

WINDOW *info_win;

void clear_window(WINDOW *window);

void info(float TR, float TI, float TE, int potentiometer, int pid, int hysteresis) {
	clear_window(info_win);
	info_win = newwin(HEIGHT, WIDTH, 0, WIDTH);
	box(info_win, 0, 0);

  mvwprintw(info_win, 2, 30, "INFORMACOES");

	if (potentiometer) {
		mvwprintw(info_win, 4, 2, "Temperatura de Referência escolhida: POTENCIOMETRO");
	} else {
		mvwprintw(info_win, 4, 2, "Temperatura de Referência escolhida: LINHA DE COMANDO");
	}

  if (pid) {
    mvwprintw(info_win, 6, 2, "Estratégia de controle escolhida: PID");
  } else {
    mvwprintw(info_win, 6, 2, "Estratégia de controle escolhida: ON_OFF");
    mvwprintw(info_win, 7, 2, "Histerese: %d", hysteresis);
  }

  mvwprintw(info_win, 9, 2, "TR: %4.2f", TR);
	mvwprintw(info_win, 10, 2, "TI: %4.2f", TI);
	mvwprintw(info_win, 11, 2, "TE: %4.2f", TE);
	wrefresh(info_win);
}

void clear_window(WINDOW *window) {
	wclear(window);
	wborder(window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(window);
	delwin(window);
}
