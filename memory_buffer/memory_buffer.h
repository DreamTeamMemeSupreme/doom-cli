//
//  memory_buffer.h
//  str_hashmap
//
//  Created by Anton Suslov on 10/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef memory_buffer_h
#define memory_buffer_h

#include <stddef.h>

typedef struct {
    size_t value_length;
    void *value;
} memory_buffer;

void memory_buffer_copy(memory_buffer *dest, const memory_buffer *src);

void memory_buffer_delete(memory_buffer *this);

memory_buffer memory_buffer_new();

memory_buffer *memory_buffer_dup(const memory_buffer *src);

#endif /* memory_buffer_h */
