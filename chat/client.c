#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include <sys/ioctl.h>
#include <signal.h>
#include <ncurses.h>

#include "conf.h"
#include "init_chat.h"

int connect(int, char*);
int disconnect(int, int);

int connect(int id, char* name)
{
	int ret;
	struct clientctl bufsnd;
	struct clientctl bufrcv;

	bufsnd.type = CTL_TO_SERVER;
	bufsnd.act = CTL_CON;
	strcpy(bufsnd.name, name);

	ret = msgsnd(id, (void*) &bufsnd, sizeof(struct clientctl), 0);
	if (ret == -1) {
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}

	ret = msgrcv(id, (void*) &bufrcv, sizeof(struct clientctl),
			CTL_TO_CLIENT, MSG_NOERROR);
	if (ret == -1) {
		perror("msgrcv");
		exit(EXIT_FAILURE);
	}

	return bufrcv.id;
}

int disconnect(int id, int utype)
{
	int ret;
	struct clientctl bufsnd;

	bufsnd.type = CTL_TO_SERVER;
	bufsnd.act = CTL_DISCON;
	bufsnd.id = utype;

	ret = msgsnd(id, (void*) &bufsnd, sizeof(struct clientctl), 0);
	if (ret == -1) {
		perror("msgsnd");
		return -1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	int id;
	int ret;
	int act;
	int utype;
	key_t key;

	if (argc != 2) {
		printf("Name not specified\n");
		return -1;
	}

	key = ftok(filename, ch);
	if (key == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}

	id = msgget(key, 0);
	if (id == -1) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}

	utype = connect(id, argv[1]);

	WINDOW* win_user;
	WINDOW* win_text;

	init_chat(&win_text, &win_user);

	while ((act = wgetch(stdscr)) != KEY_F(10)) {
	//	ret = 
	}

	disconnect(id, utype);

	endwin();

	return 0;
}
