#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct message {
	long mtype;
	char mtext[20];
};

void rmid(int id)
{
	int ret;
	ret = msgctl(id, IPC_RMID, NULL);
	if (ret != 0) {
		perror("msgctl");
		exit(EXIT_FAILURE);
	}
}

int main()
{
	int id;
	int ret;
	key_t key;

	struct message bufrcv;
	struct message bufsnd;

	key = ftok("Makefile", 'q');
	if (key == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}

	id = msgget(key, IPC_CREAT | 0666);
	if (id == -1) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}

	bufsnd.mtype = (long) 2;
	strcpy(bufsnd.mtext, "Hi, client");

	ret = msgsnd(id, (void*) &bufsnd, sizeof(struct message), 0);
	if (ret == -1) {
		perror("msgsnd");
		rmid(id);
		exit(EXIT_FAILURE);
	}

	ret = msgrcv(id, (void*) &bufrcv, sizeof(struct message), (long) 1, MSG_NOERROR);
	if (ret == -1) {
		perror("msgrcv");
		rmid(id);
		exit(EXIT_FAILURE);
	}

	printf("%s\n", bufrcv.mtext);

	rmid(id);

	return 0;
}
