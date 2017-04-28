//
//  poll_array.c
//  doom-cli
//
//  Created by Anton Suslov on 19/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "poll_array.h"

void poll_array_init(poll_array *this, const unsigned int size, pa_error *err) {
	poll_array new_struct = {
		.descriptors = calloc(size, sizeof(poll_array)),
		.active = calloc(size, 1),
		.size = size,
		.last_cursor = 0,
		.polled = 0,
		.responsible = calloc(size, 1)
	};
	if (!new_struct.descriptors) {
		*err = PA_MEMORY_ERROR;
	} else {
		*err = PA_OK;
	}
	if (!new_struct.active) {
		*err = PA_MEMORY_ERROR;
	} else {
		*err = PA_OK;
	}
	if (!new_struct.responsible) {
		*err = PA_MEMORY_ERROR;
	} else {
		*err = PA_OK;
	}
	if (OPEN_MAX < size) {
		fprintf(stderr, "Warning: poll array size is bigger than OPEN_MAX (%d)\n", OPEN_MAX);
	}
	*this = new_struct;
}

void poll_array_delete(poll_array *this) {
	int i;
	if (this->responsible) {
		if (this->active) {
			if (this->descriptors) {
				for (i = 0; i < this->size; i++) {
					if (this->active[i] && this->responsible[i]) {
						close(this->descriptors[i].fd);
					}
				}
			}
		}
	}
	if (this->active) {
		free(this->active);
	}
	if (this->descriptors) {
		free(this->active);
	}
	if (this->responsible) {
		free(this->responsible);
	}
}

int poll_array_insert(poll_array *this, int fd, int responsible, pa_error *err) {
	return poll_array_insert_at(this, fd, 0, this->size, responsible, err);
}

int poll_array_insert_at(poll_array *this, int fd, int begin, int end, int responsible, pa_error *err) {
	int i;
	for (i = begin; i < this->size && this->active[i] && i < end; i++);
	if (i == this->size || i == end) {
		*err = PA_FD_LIMIT;
		i = -1;
	} else {
		this->active[i] = 1;
		this->descriptors[i].fd = fd;
		this->responsible[i] = responsible;
		*err = PA_OK;
	}
	return i;
}

void poll_array_remove(poll_array *this, int idx, pa_error *err) {
	this->active[idx] = 0;
	this->descriptors[idx].events = 0;
}

int poll_array_poll(poll_array *this, int timeout, pa_error *err) {
	if (this->polled <= 0) {
		this->polled = poll(this->descriptors, this->size, timeout);
		if (this->polled < 0) {
			*err = PA_UNKNOWN;
			perror(NULL);
			return -1;
		}
		if (this->polled == 0) {
			*err = PA_OK;
			return -2;
		}
		this->last_cursor = 0;
	}
	for(; this->last_cursor < this->size; this->last_cursor++) {
		if (this->descriptors[this->last_cursor].revents) {
			*err = PA_OK;
			this->polled--;
			return this->last_cursor;
		}
	}
	*err = PA_UNKNOWN;
	perror(NULL);
	return -1;
}

void poll_array_close(poll_array *this, int fd_idx, pa_error *err) {
	if (fd_idx < 0 || fd_idx >= this->size || !this->active[fd_idx]) {
		*err = PA_BAD_CLIENT;
		return;
	}
	close(this->descriptors[fd_idx].fd);
	this->descriptors[fd_idx].events = 0;
	this->active[fd_idx] = 0;
	*err = PA_OK;
}
