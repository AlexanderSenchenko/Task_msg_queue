#ifndef CONF_H
#define CONF_H

#define CTL_CON 10
#define CTL_DISCON 11
#define CTL_ENAME 12

#define MSG_SND 20
#define MSG_RCV 21
#define MSG_CON 22
#define MSG_DISCON 23
#define MSG_USER_LIST 24

const long CTL_TO_SERVER = (long) 10;
const long CTL_TO_CLIENT = (long) 11;

struct clientctl {
	long type;
	int act;
	int id;
	char name[16];
};

struct message {
	long type;
	int act;
	int id;
	int size;
	char message[64];
};

struct user_list {
	long type;
	int act;
	int count;
	int id[4];
	char list[4][16];
};

const char* filename = "Makefile";
const char ch = 'q';

#endif
