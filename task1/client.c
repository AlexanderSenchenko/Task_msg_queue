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

int main()
{
	int id;
	int ret;
	key_t key;

	struct message bufsnd;
	struct message bufrcv;

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

	ret = msgrcv(id, (void*) &bufrcv, sizeof(struct message), (long) 2, MSG_NOERROR);
	if (ret == -1) {
		perror("msgrcv");
		exit(EXIT_FAILURE);
	}

	printf("%s\n", bufrcv.mtext);

	bufsnd.mtype = (long) 1;
	strcpy(bufsnd.mtext, "Hello server");

	ret = msgsnd(id, (void*) &bufsnd, sizeof(struct message), 0);
	if (ret == -1) {
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}

	return 0;
}
