#ifndef ACT_H
#define ACT_H

void connect(char*);
int disconnect();
void get_user_list(struct message);
void* msg_wait(void*);
void print_user();

#endif
