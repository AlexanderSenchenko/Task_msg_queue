#ifndef LIST_H
#define LIST_H

struct list {
	char name[16];
	int id;
	struct list *next;
};

struct list* create_node(char*, int);
int add_node(struct list**, char*, int);
int del_node(struct list**, int);
struct list* search_node(struct list*, int);
void free_list(struct list*);

#endif
