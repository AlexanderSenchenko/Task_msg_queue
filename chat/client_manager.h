#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

void client_manager();
static void client_connect(struct message*, struct list**);
static void client_disconnect(struct message*, struct list**);

#endif
