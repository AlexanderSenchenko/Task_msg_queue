#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <ncurses.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "list.h"
#include "conf.h"
#include "act.h"

extern int id;
long utype;

struct list* head;
int count_users = 0;

extern WINDOW* win_text;
extern WINDOW* win_user;

void* msg_wait(void* ptr)
{
	int ret;	
	struct message buf;

	while (1) {
		ret = msgrcv(id, (void*) &buf, sizeof(struct message),
				utype, MSG_NOERROR);

		if (ret == -1) {
			if (errno == EIDRM) {
				perror("msgrcv");
				break;
			}
			if (errno == ENOMSG)
				continue;
			perror("msgrcv");
			continue;
		}

		switch (buf.act) {
			case MSG_USER_LIST:
				get_user_list(buf);
				break;
			case MSG_RCV:
				break;
			default:
				break; 
		}
	}

	return NULL;
}

void get_user_list(struct message buf)
{
	int i;
	char name[16];
	for (i = 0; i < buf.size; ++i) {
		strncpy(name, &(buf.message[i * 16]), 16);

		add_node(&head, name, -1);

		wmove(win_user, count_users, 0);
		wprintw(win_user, "%s", name);

		count_users++;
		memset(name, 0, 16);
	}
	wrefresh(win_user);
}

void print_user()
{
	struct list* node = head;
	wmove(win_user, 0, 0);
	while (node != NULL) {
		wprintw(win_user, "%s\n", node->name);
		node = node->next;
	}
}

void connect(char* name)
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

	utype = (long) bufrcv.id;
}

int disconnect()
{
	int ret;
	struct clientctl bufsnd;

	free_list(head);

	bufsnd.type = CTL_TO_SERVER;
	bufsnd.act = CTL_DISCON;
	bufsnd.id = (int) utype;

	ret = msgsnd(id, (void*) &bufsnd, sizeof(struct clientctl), 0);
	if (ret == -1) {
		perror("msgsnd");
		return -1;
	}

	return 0;
}
