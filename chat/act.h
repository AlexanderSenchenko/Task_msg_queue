#ifndef ACT_H
#define ACT_H

void recive_message(struct message);
void send_message(char*);
void add_sym(char*, int);
void del_sym(char*);
void connect(char*);
int disconnect();
void add_user(struct message);
void del_user(struct message);
void* msg_wait(void*);
void print_user();

#endif
