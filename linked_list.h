#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "linked_list.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

struct linked_list_node {
	void* data;
	struct linked_list_node* next;
};

void linked_list_node_free(struct linked_list_node* node);

struct linked_list {
	struct linked_list_node* head;
	struct linked_list_node* tail;
	struct linked_list_node* current;
	size_t size;
};

void linked_list_initialize(struct linked_list* list, size_t size);

void linked_list_add(struct linked_list* list, void* data);
void linked_list_delete(struct linked_list* list, struct linked_list_node* node);

void* linked_list_next(struct linked_list* list);
void linked_list_reset(struct linked_list* list);

void linked_list_print(struct linked_list* list);

void linked_list_free(struct linked_list* list);

#endif // LINKED_LIST_H
