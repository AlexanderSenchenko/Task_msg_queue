#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
// #include <unistd.h>
// #include <errno.h>
// #include <string.h>

// #include "conf.h"
#include "list.h"
// #include "client_manager.h"

void rmid();
void sighandler(int);
void client_manager();

int id;
int count_clients = 0;
extern const char* filename;
extern const char ch;

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
