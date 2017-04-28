//
//  memory_buffer.c
//  str_hashmap
//
//  Created by Anton Suslov on 10/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include "string.h"
#include "stdlib.h"
#include "memory_buffer.h"

void memory_buffer_copy(memory_buffer *dest, const memory_buffer *src) {
	dest->value = malloc(src->value_length);
	memcpy(dest, src, src->value_length);
	dest->value_length = src->value_length;
}

memory_buffer memory_buffer_new() {
	memory_buffer result = {NULL, 0};
	return result;
}

void memory_buffer_delete(memory_buffer *this) {
	free(this->value);
	this->value = NULL;
	this->value_length = 0;
}
