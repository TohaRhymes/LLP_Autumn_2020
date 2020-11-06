#include "list.h"

list* list_create(int value) {
	return list_add_front(value, NULL);
}

list* list_add_front(int value, list* link) {
	list* new_node;
	new_node = malloc(sizeof(list));
	new_node->value = value;
	new_node->next = link;
	return new_node;
}

void list_add_back(int value, list* link) {
	list* new_node;


	while (link->next != NULL) {
		link = link->next;
	}

	new_node = malloc(sizeof(link));
	new_node->value = value;
	new_node->next = NULL;
	link->next = new_node;
}

int list_get(list *link, int index) {
	link = list_node_at(link, index);
	if (NULL == link)
		return 0;
	else
		return link->value;
}

void free_list(list*link) {
	list* forFree;
	while (link != NULL) {
		forFree = link;
		link = link->next;
		free(forFree);
	}
}

int list_length(list* link) {
	long length = 0;
	while (link->next != NULL) {
		length++;
		link = link->next;
	}
	return length;
}

list* list_node_at(list *link, int index) {
	int i = 0;
	while (i < index && link->next != NULL) {
		link = link->next;
		i++;
	}
	if (i == index)
		return link;
	else
		return NULL;
}

long list_sum(list *link) {
	long sum = 0;
	while (link->next != NULL) {
		sum = sum + link->value;
		link = link->next;
	}
	return sum;
}
