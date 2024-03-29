#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <ncurses.h>

#include <pthread.h>

#include "init_chat.h"

int id;
char name[16];

WINDOW* win_user;
WINDOW* win_text;
WINDOW* win_msg;

void send_message(char*);
void add_sym(char*, int);
void del_sym(char*);
void* msg_wait(void*);
void connect(char*);
int disconnect();

int main(int argc, char** argv)
{
	int act;
	key_t key;

	if (argc != 2) {
		printf("Name not specified\n");
		return -1;
	}

	strcpy(name, argv[1]);

	key = ftok("Makefile", 'q');
	if (key == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}

	id = msgget(key, 0);
	if (id == -1) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}

	connect(argv[1]);

	init_chat(&win_text, &win_user, &win_msg);

	pthread_t tid;

	pthread_create(&tid, NULL, msg_wait, NULL);

	char buf[64];

	wmove(win_text, 0, 0);
	while ((act = wgetch(stdscr)) != KEY_F(10)) {
		switch (act) {
			case 10: // Enter
				send_message(buf);
				break;
			case KEY_BACKSPACE:
				del_sym(buf);
				break;
			default:
				add_sym(buf, act);
				break;
		}
	}

	pthread_cancel(tid);
	
	disconnect();

	endwin();

	return 0;
}
