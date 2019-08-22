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
	int size_msg = sizeof(struct message);
	struct message msg; 

	struct list* head = NULL;
	struct list* node = NULL;

	while (1) {
		ret = msgrcv(id, (void*) &msg, size_msg, CTL_TO_SERVER,
								MSG_NOERROR);
		if (ret == -1) {
			if (errno == EIDRM) {
				perror("msgrcv");
				break;
			}
			perror("msgrcv");
			continue;
		}

		switch (msg.act) {
			case CTL_CON:
				client_connect(&msg, &head);
				break;
			case CTL_DISCON:
				client_disconnect(&msg, &head);
				break;
			case MSG_SND:
				broadcast_message(&msg, head);
				break;
			default:
				break;
		}
	}

	free_list(head);
}

static void client_connect(struct message* msg, struct list** head)
{
	int cltype;
	struct clientctl* ptr = (struct clientctl*) msg;

	count_clients++;
	count++;

	cltype = count + 100;

	add_node(head, ptr->name, cltype);

	printf("Count clients: %d\n", count_clients);
	printf("%s connent\n", ptr->name);

	snd_conf_info(cltype);

	snd_list_user(*head, cltype);

	struct clientctl snd;

	snd.act = MSG_CON;
	snd.id = cltype;
	strncpy(snd.name, ptr->name, 16);

	broadcast(*head, cltype, (void*) &snd, sizeof(struct clientctl));
}

static void client_disconnect(struct message* msg, struct list** head)
{
	struct clientctl* ptr;
	struct list* node;

	ptr = (struct clientctl*) msg;

	count_clients--;

	node = search_node_id(*head, ptr->id);
	printf("Count clients: %d\n", count_clients);
	printf("%s disconnect\n", node->name);

	struct clientctl snd;

	snd.act = MSG_DISCON;
	snd.id = node->id;

	broadcast(*head, ptr->id, (void*) &snd, sizeof(struct clientctl));

	del_node_id(head, ptr->id);

	#if 0
	node = *head;
	while (node != NULL) {
		msg_snd.type = (long) node->id;

		ret = msgsnd(id, (void*) &msg_snd, sizeof(struct message), 0);
		if (ret == -1)
			perror("msgsnd");

		node = node->next;
	}
	#endif
}

static void broadcast_message(struct message* msg, struct list* head)
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

static void broadcast(struct list* head, int cltype, void* msg, int size)
{
	struct list* node = head;
	struct clientctl* ptr = (struct clientctl*) msg;
	int ret;

	while (node != NULL) {
		if (node->id == cltype) {
			node = node->next;
			continue;
		}

		ptr->type = (long) node->id;

		ret = msgsnd(id, (void*) ptr, size, 0);
		if (ret == -1)
			perror("msgsnd");

		node = node->next;
	}
}

static void snd_list_user(struct list* head, int cltype)
{
	struct list* node = head;
	struct user_list msg;
	int i;
	int ret;

	msg.type = (long) cltype;
	msg.act = MSG_USER_LIST;

	while (node != NULL) {
		for (i = 0; (node != NULL) && (i < 4); ++i) {
			msg.id[i] = node->id;
			strcpy(msg.list[i], node->name);
			node = node->next;
		}

		msg.count = i;

		ret = msgsnd(id, (void*) &msg, sizeof(struct user_list), 0);
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
