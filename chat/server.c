#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "conf.h"
#include "list.h"

void rmid();
void sighandler(int);
void client_manager();

int id;

void sighandler(int sig)
{
	rmid();
	exit(EXIT_SUCCESS);
}

void rmid()
{
	int ret;
	ret = msgctl(id, IPC_RMID, NULL);
	if (ret != 0) {
		perror("msgctl");
		exit(EXIT_FAILURE);
	}
}

void client_manager()
{
	int ret;
	int cltype;
	int count_clients = 0;
	struct clientctl bufrcv;
	struct clientctl bufsnd;

	struct list* head = NULL;
	struct list* node = NULL;

	while (1) {
		ret = msgrcv(id, (void*) &bufrcv, sizeof(struct clientctl),
			     CTL_TO_SERVER, MSG_NOERROR);
		if (ret == -1) {
			if (errno == EIDRM) {
				perror("msgrcv");
				break;
			}
			perror("msgrcv");
			continue;
		}

		if (bufrcv.act == CTL_CON) {
			count_clients++;
			cltype = count_clients + 100;

			add_node(&head, bufrcv.name, cltype);

			printf("Count clients: %d\n", count_clients);
			printf("%s connent\n", bufrcv.name);

			bufsnd.type = CTL_TO_CLIENT;
			bufsnd.id = cltype;

			ret = msgsnd(id, (void*) &bufsnd,
				     sizeof(struct clientctl), 0);
			if (ret == -1)
				perror("msgsnd");
		} else if (bufrcv.act == CTL_DISCON) {
			count_clients--;

			node = search_node(head, bufrcv.id);
			printf("Count clients: %d\n", count_clients);
			printf("%s disconnect\n", node->name);

			del_node(&head, bufrcv.id);
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

int main()
{
	int ret;
	key_t key;
	pid_t pid;

	key = ftok(filename, ch);
	if (key == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}

	id = msgget(key, IPC_CREAT | 0666);
	if (id == -1) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, sighandler);

	#if 0
	pid = fork();
	if (pid == 0) {
		client_manager();
	} else {
		while (1) {
			ret = msgrcv(id, (void
		}
	}
	#else
	client_manager();
	#endif


	return 0;
}
