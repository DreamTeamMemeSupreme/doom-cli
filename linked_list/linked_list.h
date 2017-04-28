//
//  linked_list.h
//  str_hashmap
//
//  Created by Anton Suslov on 25/03/17.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef linked_list_h
#define linked_list_h

#include "../memory_buffer/memory_buffer.h"

typedef struct linked_list_node {
    struct linked_list_node* next;
    memory_buffer value;
} linked_list_node;

typedef struct {
    int size;
    linked_list_node *first;
    linked_list_node *last;
} linked_list;

void linked_list_init(linked_list *this);

void linked_list_delete(linked_list *this);

void linked_list_push_back(linked_list *this, const memory_buffer *data);

void linked_list_push_front(linked_list *this, const memory_buffer *data);

memory_buffer *linked_list_pop_front(linked_list *this);

#endif /* linked_list_h */
