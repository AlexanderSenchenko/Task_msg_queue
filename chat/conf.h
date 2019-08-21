#ifndef CONF_H
#define CONF_H

#define CTL_CON 10
#define CTL_DISCON 11
#define CTL_ENAME 12

#define MSG_ADD_USER 20
#define MSG_DEL_USER 21
#define MSG_DISCON 22
#define MSG_SND 23
#define MSG_RCV 24

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
	int size;
	char message[64];
};

const char* filename = "Makefile";
const char ch = 'q';

#endif
