#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

void client_manager();
static void client_connect(struct message*, struct list**);
static void client_disconnect(struct message*, struct list**);
static void broadcast_message(struct message*, struct list*);
static void broadcast(struct list*, int, void*, int);
static void snd_list_user(struct list*, int);
static void snd_conf_info(int);

#endif
