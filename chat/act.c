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
extern WINDOW* win_msg;

int size_buf = 0;

int text_row = 0;

void recive_message(struct message msg)
{
	char* str = malloc(sizeof(char) * msg.size);
	strncpy(str, msg.message, msg.size);

	wmove(win_text, text_row, 0);
	wprintw(win_text, "%s", str);
	wrefresh(win_text);

	text_row++;
	free(str);
}

void send_message(char* buf)
{
	int ret;
	struct message msg;

	buf[size_buf++] = '\0';

	msg.type = CTL_TO_SERVER;
	msg.act = MSG_SND;
	msg.id = (int) utype;
	msg.size = size_buf;
	strncpy(msg.message, buf, size_buf);

	ret = msgsnd(id, (void*) &msg, sizeof(struct message), 0);
	if (ret == -1) {
		perror("msgsnd");
		return;
	}

	wmove(win_text, text_row, 0);
	wprintw(win_text, "%s", buf);
	wrefresh(win_text);

	text_row++;

	wmove(win_msg, 0, 0);
	whline(win_msg, ' ', 64);
	wrefresh(win_msg);

	memset(buf, 0, size_buf);
	size_buf = 0;
}

void add_sym(char* buf, int ch)
{
	if (size_buf == 62)
		return;

	wmove(win_msg, 0, size_buf);
	wprintw(win_msg, "%c", ch);
	wrefresh(win_msg);

	buf[size_buf++] = ch;
}

void del_sym(char* buf)
{
	if (size_buf == 0)
		return;

	wmove(win_msg, 0, size_buf - 1);
	wprintw(win_msg, "%c", ' ');
	wrefresh(win_msg);

	buf[size_buf--] = 0;
}

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
			case MSG_ADD_USER:
				add_user(buf);
				break;
			case MSG_DEL_USER:
				del_user(buf);
				break;
			case MSG_RCV:
				recive_message(buf);
				break;
			default:
				break; 
		}
	}

	return NULL;
}

void add_user(struct message buf)
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

void del_user(struct message buf)
{
	del_node_name(&head, buf.message);
	count_users--;

	wclear(win_user);

	print_user();
}

void print_user()
{
	struct list* node = head;
	wmove(win_user, 0, 0);
	while (node != NULL) {
		wprintw(win_user, "%s\n", node->name);
		node = node->next;
	}
	wrefresh(win_user);
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
