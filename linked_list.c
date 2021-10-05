#include "linked_list.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void linked_list_node_free(struct linked_list_node* node)
{
	assert(node != NULL);

	if (node->data != NULL)
		free(node->data);
	free(node);
}

void linked_list_initialize(struct linked_list* list, size_t size)
{
	list->head = NULL;
	list->current = NULL;
	list->tail = NULL;
	list->size = size;
}

void linked_list_add(struct linked_list* list, void* data)
{
	assert(list->size != 0);

	struct linked_list_node* node = (struct linked_list_node*)malloc(sizeof(struct linked_list_node));
	if (node == NULL) {
		fprintf(stderr, "error allocating memory for linked list\n");
		exit(1);
	}
	
	node->data = (void*)malloc(list->size);
	if (node->data == NULL) {
		fprintf(stderr, "error allocating memory for linked list node\n");
		exit(1);
	}

	memcpy(node->data, data, list->size);
	node->next = NULL;
	
	if (list->tail != NULL)
		list->tail->next = node;
	list->tail = node;
	
	if (list->head == NULL) {
		list->head = node;
		list->current = list->head;
	}
}

void linked_list_delete(struct linked_list* list, struct linked_list_node* node)
{
	struct linked_list_node* current_node = list->head;
	struct linked_list_node* previous_node = NULL;
	while (current_node != NULL) {
			if (current_node == node) {
				if (current_node == list->head)
					list->head = list->head->next;
				else
					previous_node->next = current_node->next;
					
				linked_list_node_free(current_node);
				break;
			}
			
			previous_node = current_node;
			current_node = current_node->next;
	}
}

void* linked_list_next(struct linked_list* list)
{
	void* data = NULL;

	if (list->current != NULL) {
		data = list->current->data;
		list->current = list->current->next;
	}
	
	return data;
}

void linked_list_reset(struct linked_list* list)
{
	list->current = list->head;
}

void linked_list_print(struct linked_list* list)
{
	struct linked_list_node* current_node = list->head;
	while (current_node != NULL) {
		for (int i = 0; i < list->size; ++i) {
			uint8_t* value = ((uint8_t*)current_node->data + i);
			printf("0x%x ", *value);
		}
		printf("\n");
		current_node = current_node->next;
	}
	printf("\n");
}

void linked_list_free(struct linked_list* list)
{
	struct linked_list_node* current_node = list->head;
	while (current_node != NULL) {
		struct linked_list_node* next_node = current_node->next;
		free(current_node);
		current_node = next_node;
	}
}
