//
//  linked_list.c
//  str_hashmap
//
//  Created by Anton Suslov on 25/03/17.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include "linked_list.h"
#include "linked_list-PRIVATE.h"
#include "stdlib.h"
#include "string.h"

void linked_list_node_init(linked_list_node *this) {
	this->next = NULL;
	this->value = memory_buffer_new();
}

void linked_list_node_delete(linked_list_node *this) {
	if (!this) {
		return;
	}
	free(this->value.value);
}

void linked_list_init(linked_list *this) {
	this->first = NULL;
	this->last = NULL;
	this->size = 0;
}

void linked_list_delete(linked_list *this) {
	if (!this->first) {
		return;
	}
	linked_list_node *to_delete = this->first;
	linked_list_node *cursor;
	for(cursor = this->first.next; cursor; cursor = cursor->next) {
		linked_list_node_delete(to_delete);
		free(to_delete);
		to_delete = cursor;
	}
	linked_list_node_delete(to_delete);
	free(to_delete);
}

void linked_list_push_front(linked_list *this, const memory_buffer *data) {
	linked_list_node *new = malloc(sizeof(linked_list_node));
	linked_list_node_init(new);
	new->value = *data;
	if (!this->first) {
		this->last = this->first = new;
	} else {
		new->next = this->first;
		this->first = new;
	}
	this->size++;
}

void linked_list_pop_front(linked_list *this) {
	linked_list_node *to_delete;
	if (!this->first) {
		return;
	}
	to_delete = this->first;
	this->first = to_delete->next;
	if(!this->first) {
		this->last = NULL;
	}
	linked_list_node_delete(to_delete);
	this->size--;
}

void linked_list_push_back(linked_list *this, const memory_buffer *data) {
	linked_list_node *new = malloc(sizeof(linked_list_node));
	linked_list_node_init(new);
	new->value = *data;
	if (!this->first) {
		this->last = this->first = new;
	} else {
		this->last->next = new;
	}
	this->size++;
}

