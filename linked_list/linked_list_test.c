//
//  linked_list_test.c
//  doom-cli
//
//  Created by Anton Suslov on 04/05/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include <assert.h>
#include <stdlib.h>
#include "linked_list.h"

void linked_list_test_init() {
	linked_list list;
	linked_list_init(&list);
	assert(list.size == 0);
	linked_list_delete(&list);
}

void linked_list_test_empty_pop() {
	linked_list list;
	linked_list_init(&list);
	assert(linked_list_pop_front(&list) == NULL);
	linked_list_delete(&list);
}

void linked_list_test_push_back() {
	linked_list list;
	int c1 = 1;
	int c2 = 2;
	memory_buffer buf1 = {sizeof(int), &c1};
	memory_buffer buf2 = {sizeof(int), &c2};
	linked_list_init(&list);
	linked_list_push_back(&list, &buf1);
	linked_list_push_back(&list, &buf2);
	assert(list.first->value == &buf1);
	assert(list.last->value == &buf2);
	assert(linked_list_pop_front(&list) == &buf1);
	assert(list.last == list.first);
	assert(linked_list_pop_front(&list) == &buf2);
	assert(list.last == list.first);
}

int main() {
	linked_list_test_init();
	linked_list_test_push_back();
	linked_list_test_empty_pop();
}
