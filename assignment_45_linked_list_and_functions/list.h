#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct list {
	int value;
	struct list* next;
} list;

list* list_create(int);
list* list_add_front(int, list*);
void list_add_back(int, list*);
int list_get(list*, int);
void free_list(list*);
int list_length(list*);
list* list_node_at(list*, int);
long list_sum(list*);
