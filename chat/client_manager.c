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

static void client_connect(struct message* msg, struct list** head)
{
	struct clientctl* ptr;
	struct clientctl bufsnd;
	struct list* node;
	struct message msg_snd;

	int cltype;
	int ret;
	int i;

	ptr = (struct clientctl*) msg;

	count_clients++;
	count++;

	cltype = count + 100;

	add_node(head, ptr->name, cltype);

	printf("Count clients: %d\n", count_clients);
	printf("%s connent\n", ptr->name);

	bufsnd.type = CTL_TO_CLIENT;
	bufsnd.id = cltype;

	ret = msgsnd(id, (void*) &bufsnd, sizeof(struct clientctl), 0);
	if (ret == -1)
		perror("msgsnd");

	msg_snd.type = (long) cltype;
	msg_snd.act = MSG_USER_LIST;

	node = *head;
	printf("Pack:\n");
	while (node != NULL) {
		for (i = 0; (node != NULL) && (i < 4); ++i) {
			strcpy(&(msg_snd.message[16 * i]), node->name);
			printf("%s\n", node->name);
			node = node->next;
		}

		msg_snd.size = i;

		ret = msgsnd(id, (void*) &msg_snd, sizeof(struct message), 0);
		if (ret == -1)
			perror("msgsnd");
	}
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

	del_node(head, ptr->id);
}

void client_manager()
{
	int ret;
	struct message msgbuf;

	struct list* head = NULL;
	struct list* node = NULL;

	while (1) {
		ret = msgrcv(id, (void*) &msgbuf, sizeof(struct message),
				CTL_TO_SERVER, MSG_NOERROR);
		printf("Size message %d\n", ret);
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
			default:
				break;
		}

		node = head;
		printf("List clients:\n");
		while (node != NULL) {
			printf("Id %d Name: %s\n", node->id, node->name);
			node = node->next;
		}
	}

	free_list(head);
}
