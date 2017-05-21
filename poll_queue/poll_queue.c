//
//  poll_queue.c
//  doom-cli
//
//  Created by Anton Suslov on 28/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include "poll_queue.h"
#include "poll_queue-PRIVATE.h"
#include "stdlib.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void poll_queue_init(poll_queue *this, unsigned int size, pq_error *err) {
	pa_error parent_status;
	int i;
	poll_array_init((poll_array *) this, size, &parent_status);
	this->queues = NULL;
	this->pending = 0;
	if (parent_status != PA_OK) {
		*err = PQ_ERROR;
		return;
	}
	if ((this->queues = calloc(size, sizeof(linked_list))) == NULL) {
		*err = PQ_ERROR;
		return;
	}
	for (i = 0; i < size; i++) {
		linked_list_init(&this->queues[i]);
	}
	*err = PQ_OK;
}

void poll_queue_delete(poll_queue *this) {
	if (this->queues) {
		free(this->queues);
	}
	poll_array_delete((poll_array *) this);
}

void poll_queue_send(poll_queue *this, int client, const memory_buffer *data, pq_error *err) {
	memory_buffer *dup = memory_buffer_dup(data);
	if (!dup) {
		*err = PQ_ERROR;
		return;
	}
	if (!poll_queue_valid_idx(this, client)) {
		*err = PQ_BAD_CLIENT;
		return;
	}
	
	linked_list_push_back(&this->queues[client], dup);
	this->descriptors[client].events |= POLLOUT;
	*err = PQ_OK;
}

int poll_queue_valid_idx(poll_queue *this, int idx) {
	return (idx >= 0 && idx < this->size);
}

void poll_queue_send_in_range(poll_queue *this, int from, int to, const memory_buffer *data, pq_error *err) {
	int i;
	if (!poll_queue_valid_idx(this, from) || to > this->size) {
		*err = PQ_BAD_CLIENT;
		return;
	}
	for (i = from; i < to; i++) {
		poll_queue_send(this, i, data, err);
	}
}

int poll_queue_poll(poll_queue *this, int timeout, pq_error *err) {
	int client;
	int file_status;
	int fd;
	pa_error parent_error;
	memory_buffer *buf;
	client = poll_array_poll((poll_array *)this, timeout, &parent_error);
	fd = this->descriptors[client].fd;
	if (parent_error != PA_OK) {
		*err = PQ_ERROR;
		return client;
	}
	if (this->descriptors[client].revents & POLLOUT) {
		file_status = fcntl(fd, F_GETFD);
		if (file_status < 0) {
			*err = PQ_ERROR;
			return -1;
		}
		if (fcntl(fd, F_SETFL, file_status | O_NONBLOCK) == -1) {
			*err = PQ_ERROR;
			return -1;
		}
		while ((buf = linked_list_pop_front(&this->queues[client])) != NULL) {
			if (write(fd, buf->value, buf->value_length) < 0) {
				if (errno == EWOULDBLOCK) {
					linked_list_push_front(&this->queues[client], buf);
					if (fcntl(fd, F_SETFL, file_status) == -1) {
						*err = PQ_ERROR;
						return -1;
					}
					return poll_queue_poll(this, timeout, err);
				} else {
					perror("Poll Queue write");
					*err = PQ_ERROR;
					memory_buffer_delete(buf);
					free(buf);
					return -1;
				}
			}
			memory_buffer_delete(buf);
			free(buf);
			printf("Sent packet\n");
		}
		this->descriptors[client].events &= ~POLLOUT;
		if (fcntl(fd, F_SETFL, file_status) == -1) {
			*err = PQ_ERROR;
			return -1;
		}
		return poll_queue_poll(this, timeout, err);
	}
	*err = PQ_OK;
	return client;
}

void poll_queue_close(poll_queue *this, int idx, pq_error *err) {
	if (this->queues[idx].first != NULL) {
		*err = PQ_NOT_FLUSHED;
		return;
	}
	poll_queue_force_close(this, idx, err);
	*err = PQ_OK;
}

void poll_queue_force_close(poll_queue *this, int idx, pq_error *err) {
	pa_error parent_error;
	linked_list_delete(&this->queues[idx]);
	poll_array_close((poll_array *)this, idx, &parent_error);
	if (parent_error != PA_OK) {
		*err = PQ_ERROR;
		return;
	}
	linked_list_init(&this->queues[idx]);
	*err = PQ_OK;
}

int poll_queue_insert_at(poll_queue *this, int fd, int begin, int end, int responsible, pq_error *err) {
	pa_error parent_error;
	int client = poll_array_insert_at((poll_array *)this, fd, begin, end, responsible, &parent_error);
	if (parent_error != PA_OK) {
		if (parent_error == PA_BAD_CLIENT) {
			*err = PQ_BAD_CLIENT;
		} else {
			*err = PQ_ERROR;
		}
		return -1;
	}
	linked_list_init(&this->queues[client]);
	*err = PQ_OK;
	return client;
}

void poll_queue_flush_one(poll_queue *this, int idx, pq_error *err) {
	memory_buffer *buf;
	int fd = this->descriptors[idx].fd;
	int file_status = fcntl(fd, F_GETFD);
	if (file_status < 0) {
		*err = PQ_ERROR;
	}
	if (fcntl(fd, F_SETFL, file_status & ~O_NONBLOCK) == -1) {
		*err = PQ_ERROR;
		return;
	}
	while ((buf = linked_list_pop_front(&this->queues[idx])) != NULL) {
		if (write(fd, buf->value, buf->value_length) < 0) {
			*err = PQ_ERROR;
		}
		memory_buffer_delete(buf);
		free(buf);
	}
	*err = PQ_OK;
}

void poll_queue_flush_all(poll_queue *this, pq_error *err) {
	int i = 0;
	for (i = 0; i < this->size; i++) {
		if (this->active[i]) {
			poll_queue_flush_one(this, i, err);
			if (*err != PA_OK) {
				return;
			}
		}
	}
	*err = PQ_OK;
}

int poll_queue_detach(poll_queue *this, int client, pq_error *err) {
	int to_return;
	if (!poll_queue_valid_idx(this, client) || !this->active[client]) {
		*err = PQ_BAD_CLIENT;
		return -1;
	}
	if (this->queues[client].first) {
		*err = PQ_NOT_FLUSHED;
		return -1;
	}
	linked_list_delete(&this->queues[client]);
	to_return = this->descriptors[client].fd;
	this->descriptors[client].fd = 0;
	this->descriptors[client].events = 0;
	this->active[client] = 0;
	return to_return;
}

int poll_queue_insert(poll_queue *this, int fd, int client, int responsible, pq_error *err) {
	return poll_queue_insert_at(this, fd, client, client+1, responsible, err);
}

