#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include "dashboard.h"
#include "main.h"

int startx = 0;
int starty = 0;
WINDOW *menu_win, *info_win;

char *choices[] = {
	"1) Utilizar referência do potenciômetro",
	"2) Definir temperatura de referência pela linha de comando",
	"3) Ativar/Desativar estratégia de controle via botão físico",
	"4) Definir estratégia de controle via PID",
	"5) Definir estratégia de controle via ON_OFF",
	"6) Sair",
};

int n_choices = sizeof(choices) / sizeof(char *);
void print_menu(WINDOW *menu_win, int highlight);
void clear_window(WINDOW *window);
void new_temperature_reference();
void new_hysteresis();
void new_pid_constants();
void clear_window(WINDOW *window);

void info(float TR, float TI, float TE, int potentiometer, int pid, float Kp, float Ki, float Kd, int hysteresis, int use_key_switch) {
	clear_window(info_win);
	info_win = newwin(HEIGHT, WIDTH, 0, WIDTH + 2);
	box(info_win, 0, 0);

  mvwprintw(info_win, 2, 31, "INFORMACOES");

	if (potentiometer) {
		mvwprintw(info_win, 4, 2, "Temperatura de Referência definida: POTENCIOMETRO");
	} else {
		mvwprintw(info_win, 4, 2, "Temperatura de Referência definida: LINHA DE COMANDO");
	}

	if (use_key_switch) {
		mvwprintw(info_win, 6, 2, "Chave Externa: ATIVADA");
	} else {
		mvwprintw(info_win, 6, 2, "Chave Externa: DESATIVADA");
	}

  if (pid) {
    mvwprintw(info_win, 7, 2, "Estratégia de controle definida: PID");
    mvwprintw(info_win, 8, 2, "Kp: %4.2f | Ki: %4.2f | Kd: %4.2f", Kp, Ki, Kd);
  } else {
    mvwprintw(info_win, 7, 2, "Estratégia de controle definida: ON_OFF");
    mvwprintw(info_win, 8, 2, "Histerese: %d", hysteresis);
  }

  mvwprintw(info_win, 10, 2, "TR: %4.2f", TR);
	mvwprintw(info_win, 11, 2, "TI: %4.2f", TI);
	mvwprintw(info_win, 12, 2, "TE: %4.2f", TE);
	wrefresh(info_win);
}

void menu() {
	int highlight = 1;
	int choice = 0;
	int c;

	initscr();
	clear();
	noecho();
	cbreak();
	curs_set(0);
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_BLACK);

	menu_win = newwin(HEIGHT, WIDTH, 0, 0);
	keypad(menu_win, TRUE);
	refresh();

	while (1) {
		print_menu(menu_win, highlight);
		c = wgetch(menu_win);

		switch (c) {
			case KEY_UP:
				if (highlight == 1)
					highlight = n_choices;
				else
					--highlight;
				break;

			case KEY_DOWN:
				if (highlight == n_choices)
					highlight = 1;
				else
					++highlight;
				break;

			case 10:
				choice = highlight;
				if (choice == 1) {
					set_potentiometer(1);
				}
				
				if (choice == 2) {
					clear_window(menu_win);
					menu_win = newwin(HEIGHT, WIDTH, 0, 0);
					set_potentiometer(0);
					new_temperature_reference();
					keypad(menu_win, TRUE);
					refresh();
				} else if (choice == 3) {
					set_use_key_switch();
					break;
				} else if (choice == 4) {
					new_pid_constants();
					set_pid(1);
					break;
				} else if (choice == 5) {
					clear_window(menu_win);
					menu_win = newwin(HEIGHT, WIDTH, 0, 0);
					new_hysteresis();
					set_pid(0);
					keypad(menu_win, TRUE);
					refresh();
				} else if (choice == 6) {
					shut_down_system();
				}

				break;

			default:
				refresh();
				break;
		}
	};
}

void print_menu(WINDOW *menu_win, int highlight) {
	int x, y, i;

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for (i = 0; i < n_choices; ++i)
	{
		if (highlight == i + 1) /* High light the present choice */
		{
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

void clear_window(WINDOW *window) {
	wclear(window);
	wborder(window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(window);
	delwin(window);
}

void new_temperature_reference() {
	WINDOW *local_window;
	float temperature_reference;
	int max_height, max_width;
	int _startx, _starty;

	getmaxyx(stdscr, max_height, max_width);

	_starty = (LINES - max_height) / 2;
	_startx = (COLS - max_width) / 2;
	refresh();

	local_window = newwin(HEIGHT, WIDTH, _starty, _startx);

	echo();

	wattron(local_window, COLOR_PAIR(1));
	mvwprintw(local_window, _starty + 1, _startx + 1, "Insira a temperatura de referência: ");
	wattroff(local_window, COLOR_PAIR(1));
	wmove(local_window, _starty + 1, _startx + 38);
	wscanw(local_window, "%f", &temperature_reference);

	set_temperature_reference_input(0, temperature_reference);
}

void new_hysteresis() {
	WINDOW *local_window;
	int hysteresis_input;
	int max_height, max_width;
	int _startx, _starty;

	getmaxyx(stdscr, max_height, max_width);

	_starty = (LINES - max_height) / 2;
	_startx = (COLS - max_width) / 2;
	refresh();

	local_window = newwin(HEIGHT, WIDTH, _starty, _startx);

	echo();

	wattron(local_window, COLOR_PAIR(1));
	mvwprintw(local_window, _starty + 1, _startx + 1, "Insira o valor para histerese: ");
	wattroff(local_window, COLOR_PAIR(1));
	wmove(local_window, _starty + 1, _startx + 32);
	wscanw(local_window, "%d", &hysteresis_input);
	
	set_new_hysteresis(hysteresis_input);
}

void new_pid_constants() {
	WINDOW *local_window;
	float Kp_input, Ki_input, Kd_input;
	int max_height, max_width;
	int _startx, _starty;

	getmaxyx(stdscr, max_height, max_width);

	_starty = (LINES - max_height) / 2;
	_startx = (COLS - max_width) / 2;
	refresh();

	local_window = newwin(HEIGHT, WIDTH, _starty, _startx);

	echo();

	wattron(local_window, COLOR_PAIR(1));
	mvwprintw(local_window, _starty + 1, _startx + 1, "Insira o valor para Kp: ");
	wattroff(local_window, COLOR_PAIR(1));
	wmove(local_window, _starty + 1, _startx + 25);
	wscanw(local_window, "%f", &Kp_input);

	wattron(local_window, COLOR_PAIR(1));
	mvwprintw(local_window, _starty + 2, _startx + 1, "Insira o valor para Ki: ");
	wattroff(local_window, COLOR_PAIR(1));
	wmove(local_window, _starty + 2, _startx + 25);
	wscanw(local_window, "%f", &Ki_input);

	wattron(local_window, COLOR_PAIR(1));
	mvwprintw(local_window, _starty + 3, _startx + 1, "Insira o valor para Kd: ");
	wattroff(local_window, COLOR_PAIR(1));
	wmove(local_window, _starty + 3, _startx + 25);
	wscanw(local_window, "%f", &Kd_input);

	set_new_pid_constants(Kp_input, Ki_input, Kd_input);
}
