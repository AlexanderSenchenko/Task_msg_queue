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
extern char name[16];

struct list* head;
int count_users = 0;

extern WINDOW* win_text;
extern WINDOW* win_user;
extern WINDOW* win_msg;

int size_buf = 0;

int text_row = 0;

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
	struct clientctl msg;

	free_list(head);

	msg.type = CTL_TO_SERVER;
	msg.act = CTL_DISCON;
	msg.id = (int) utype;

	ret = msgsnd(id, (void*) &msg, sizeof(struct clientctl), 0);
	if (ret == -1) {
		perror("msgsnd");
		return -1;
	}

	return 0;
}

void* msg_wait(void* ptr)
{
	int ret;
	int size_msg = sizeof(struct user_list);
	struct user_list msg;

	while (1) {
		ret = msgrcv(id, (void*) &msg, size_msg, utype, MSG_NOERROR);
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

		switch (msg.act) {
			case MSG_CON:
				connect_new_user(&msg);
				break;
			case MSG_DISCON:
				disconnect_user(&msg);
				break;
			case MSG_USER_LIST:
				recive_user_list(&msg);
				break;
			case MSG_RCV:
				recive_message(&msg);
				break;
			default:
				break; 
		}
	}

	return NULL;
}

void connect_new_user(struct user_list* msg)
{
	struct clientctl* buf = (struct clientctl*) msg;

	add_node(&head, buf->name, buf->id);

	wmove(win_user, count_users, 0);
	wprintw(win_user, "%s", buf->name);
	wrefresh(win_user);

	count_users++;
}

void disconnect_user(struct user_list* msg)
{
	struct clientctl* buf = (struct clientctl*) msg;

	del_node_id(&head, buf->id);
	count_users--;

	wclear(win_user);

	print_user();
}

void recive_user_list(struct user_list* msg)
{
	struct user_list* buf = (struct user_list*) msg;
	int i;

	for (i = 0; i < buf->count; ++i) {
		add_node(&head, buf->list[i], buf->id[i]);

		wmove(win_user, count_users, 0);
		wprintw(win_user, "%s", buf->list[i]);

		count_users++;
	}
	wrefresh(win_user);
}

void recive_message(struct user_list* msg)
{
	struct message* buf = (struct message*) msg;
	struct list* node = search_node_id(head, buf->id);
	char* str = malloc(sizeof(char) * buf->size);

	strncpy(str, buf->message, buf->size);

	wmove(win_text, text_row, 0);
	wprintw(win_text, "%s: %s", node->name, str);
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
	wprintw(win_text, "%s: %s", name, buf);
	wrefresh(win_text);

	text_row++;

	wmove(win_msg, 0, 0);
	whline(win_msg, ' ', size_buf);
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
