//
//  poll_array.h
//  doom-cli
//
//  Created by Anton Suslov on 19/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef poll_array_h
#define poll_array_h

#include <stdio.h>
#include <poll.h>
#include "../memory_buffer/memory_buffer.h"

typedef struct {
	struct pollfd *descriptors;
	char *active;
	char *responsible;
	const unsigned int size;
	int last_cursor;
	int polled;
} poll_array;

typedef enum {
	PA_OK = 0,
	PA_UNKNOWN,
	PA_MEMORY_ERROR,
	PA_FD_LIMIT,
	PA_BAD_CLIENT
} pa_error;

void poll_array_init(poll_array *this, const unsigned int size, pa_error *err);

void poll_array_delete(poll_array *this);

int poll_array_insert(poll_array *this, int fd, int responsible, pa_error *err);

int poll_array_insert_at(poll_array *this, int fd, int begin, int end, int responsible, pa_error *err);

int poll_array_poll(poll_array *this, int timeout, pa_error *err);

void poll_array_close(poll_array *this, int fd_idx, pa_error *err);
#endif /* poll_array_h */
