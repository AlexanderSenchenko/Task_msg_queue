#include <string.h>
#include <stdlib.h>

#include "list.h"

struct list* create_node(char* name, int id)
{
	struct list* head = malloc(sizeof(struct list));

	strcpy(head->name, name);
	head->id = id;
	head->next = NULL;

	return head;
}

int add_node(struct list** head, char* name, int id)
{
	if (*head == NULL) {
		*head = create_node(name, id);
		return 0;
	}

	struct list* node = *head;
	struct list* parent = NULL;

	while (node != NULL) {
		parent = node;
		node = node->next;
	}

	node = create_node(name, id);
	parent->next = node;

	return 0;
}

int del_node(struct list** head, int id)
{
	struct list* node = *head;
	struct list* parent = NULL;
	struct list* next = NULL;

	while ((node != NULL) && (node->id != id)) {
		parent = node;
		node = node->next;
	}

	if (node == NULL)
		return -1;

	next = node->next;

	if (parent != NULL) {
		parent->next = next;
	} else {
		*head = next;
	}

	// memset(node->name, 0, 16);
	// node->next = NULL;
	free(node);

	return 0;
}

struct list* search_node(struct list* head, int id)
{
	struct list* node = head;

	while ((node != NULL) && (node->id != id))
		node = node->next;

	return node;
}

void free_list(struct list* head)
{
	struct list* node;

	while (head != NULL) {
		node = head->next;
		free(head);
		head = node;
	}
}
