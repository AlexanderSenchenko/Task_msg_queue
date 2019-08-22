#ifndef ACT_H
#define ACT_H

void connect(char*);
int disconnect();

void* msg_wait(void*);

void connect_new_user(struct user_list*);
void disconnect_user(struct user_list*);
void recive_user_list(struct user_list*);
void recive_message(struct user_list*);

void send_message(char*);
void add_sym(char*, int);
void del_sym(char*);

void print_user();

#endif
