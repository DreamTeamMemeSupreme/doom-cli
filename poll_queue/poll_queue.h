//
//  poll_queue.h
//  doom-cli
//
//  Created by Anton Suslov on 28/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef poll_queue_h
#define poll_queue_h

#include "../poll_array/poll_array.h"
#include "../linked_list/linked_list.h"
#include "../memory_buffer/memory_buffer.h"

typedef enum {
	PQ_OK,
	PQ_ERROR
} pq_error;

typedef struct {
	struct pollfd *descriptors;
	char *active;
	char *responsible;
	const int size;
	int last_cursor;
	int polled;
	linked_list *queues;
} poll_queue;

void poll_queue_send(poll_queue *this, int client, memory_buffer *data, pq_error *err);

void poll_queue_send_in_range(poll_queue *this, int from, int to, memory_buffer *data, pq_error *err);

int poll_queue_poll(poll_queue *this, pq_error *err);

void poll_queue_close(poll_queue *this, pq_error *err);

int poll_queue_insert_at(poll_queue *this, int fd, int begin, int end, int responsible, pq_error *err);


#endif /* poll_queue_h */
