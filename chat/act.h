#ifndef ACT_H
#define ACT_H

void connect(char*);
int disconnect();
void add_user(struct message);
void del_user(struct message);
void* msg_wait(void*);
void print_user();

#endif
