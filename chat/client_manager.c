#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "conf.h"
#include "list.h"
#include "client_manager.h"

extern int id;
extern int count_clients;
int count = 0;

void client_manager()
{
	int ret;
	struct message msgbuf;

	struct list* head = NULL;
	struct list* node = NULL;

	while (1) {
		ret = msgrcv(id, (void*) &msgbuf, sizeof(struct message),
				CTL_TO_SERVER, MSG_NOERROR);
		if (ret == -1) {
			if (errno == EIDRM) {
				perror("msgrcv");
				break;
			}
			perror("msgrcv");
			continue;
		}

		switch (msgbuf.act) {
			case CTL_CON:
				client_connect(&msgbuf, &head);
				break;
			case CTL_DISCON:
				client_disconnect(&msgbuf, &head);
				break;
			case MSG_SND:
				broadcast_message(&msgbuf, head);
				break;
			case MSG_RCV:
				break;
			default:
				break;
		}

		#if 0
		node = head;
		printf("List clients:\n");
		while (node != NULL) {
			printf("Id %d Name: %s\n", node->id, node->name);
			node = node->next;
		}
		#endif

		memset(msgbuf.message, '0', 64);
	}

	free_list(head);
}

void broadcast_message(struct message* msg, struct list* head)
{
	struct list* node = head;
	int ret;

	msg->act = MSG_RCV;

	while (node != NULL) {
		if (node->id == msg->id) {
			node = node->next;
			continue;
		}

		msg->type = (long) node->id;

		ret = msgsnd(id, (void*) msg, sizeof(struct message), 0);
		if (ret == -1)
			perror("msgsnd");

		node = node->next;
	}
}

static void client_connect(struct message* msg, struct list** head)
{
	struct clientctl* ptr;

	int cltype;

	ptr = (struct clientctl*) msg;

	count_clients++;
	count++;

	cltype = count + 100;

	add_node(head, ptr->name, cltype);

	printf("Count clients: %d\n", count_clients);
	printf("%s connent\n", ptr->name);

	snd_conf_info(cltype);

	snd_list_user(*head, cltype);

	broadcast(*head, cltype, MSG_ADD_USER, ptr->name, 16);
}

static void broadcast(struct list* head, int cltype, int act, char* str, int size)
{
	struct list* node = head;
	struct message msg;
	int ret;

	msg.act = act;
	msg.size = 1;
	strncpy(msg.message, str, size);

	while (node != NULL) {
		if (node->id == cltype) {
			node = node->next;
			continue;
		}

		msg.type = (long) node->id;

		ret = msgsnd(id, (void*) &msg, sizeof(struct message), 0);
		if (ret == -1)
			perror("msgsnd");

		node = node->next;
	}
}

static void snd_list_user(struct list* head, int cltype)
{
	struct list* node = head;
	struct message msg;
	int i;
	int ret;

	msg.type = (long) cltype;
	msg.act = MSG_ADD_USER;

	while (node != NULL) {
		for (i = 0; (node != NULL) && (i < 4); ++i) {
			strcpy(&(msg.message[16 * i]), node->name);
			node = node->next;
		}

		msg.size = i;

		ret = msgsnd(id, (void*) &msg, sizeof(struct message), 0);
		if (ret == -1)
			perror("msgsnd");
	}
}

static void snd_conf_info(int cltype)
{
	struct clientctl msg;
	int ret;

	msg.type = CTL_TO_CLIENT;
	msg.id = cltype;

	ret = msgsnd(id, (void*) &msg, sizeof(struct clientctl), 0);
	if (ret == -1)
		perror("msgsnd");
}

static void client_disconnect(struct message* msg, struct list** head)
{
	struct clientctl* ptr;
	struct list* node;

	ptr = (struct clientctl*) msg;

	count_clients--;

	node = search_node(*head, ptr->id);
	printf("Count clients: %d\n", count_clients);
	printf("%s disconnect\n", node->name);

	int i;
	struct message msg_snd;
	struct list* node_broadcast;
	int ret;

	msg_snd.act = MSG_DEL_USER;
	strncpy(msg_snd.message, node->name, 16);
	msg_snd.size = i;

	del_node_id(head, ptr->id);

	node = *head;
	while (node != NULL) {
		msg_snd.type = (long) node->id;

		ret = msgsnd(id, (void*) &msg_snd, sizeof(struct message), 0);
		if (ret == -1)
			perror("msgsnd");

		node = node->next;
	}
}
