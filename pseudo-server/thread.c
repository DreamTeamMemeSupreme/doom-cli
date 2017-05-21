//
//  thead.c
//  doom-cli
//
//  Created by Anton Suslov on 27/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "thread-PRIVATE.h"
#include "thread.h"

void thread_server_init(thread_server *this, thread_params *args, ts_error *err) {
	pq_error poll_error;
	int flags;
	memset(this, 0, sizeof(*this));
	poll_queue_init(&this->poll, POLL_SIZE, &poll_error);
	this->params = args;
	
	if ((flags = fcntl(this->pipe_in_idx, F_GETFL)) < 0) {
		*err = TS_ERROR;
		return;
	}
	
	if (fcntl(this->pipe_in_idx, F_SETFL, flags | O_NONBLOCK) < 0) {
		*err = TS_ERROR;
		return;
	}
	
	this->pipe_in_idx = poll_queue_insert_at(&this->poll, this->params->pipe_in,
						 RESERVED_BEGIN, RESERVED_END, 1, &poll_error);
	if (poll_error != PA_OK) {
		*err = TS_ERROR;
		return;
	}
	this->poll.descriptors[this->pipe_in_idx].events = POLLIN;
	
	this->pipe_out_idx = poll_queue_insert_at(&this->poll, this->params->pipe_out,
						  RESERVED_BEGIN, RESERVED_END, 1, &poll_error);
	if (poll_error != PA_OK) {
		*err = TS_ERROR;
		return;
	}
	
	this->sigint_idx = poll_queue_insert_at(&this->poll, sigint_pipe[0],
						RESERVED_BEGIN, RESERVED_END, 0, &poll_error);
	if (poll_error != PA_OK) {
		*err = TS_ERROR;
		return;
	}
	this->poll.descriptors[this->sigint_idx].events = POLLIN;
}

void thread_server_delete(thread_server *this) {
	if (this->params->name) {
		free(this->params->name);
	}
	free(this->params);
	poll_queue_delete(&this->poll);
}

void *thread_server_f(void *param) {
	ts_error server_error;
	thread_server server;
	memset(&server, 0, sizeof(server));
	thread_server_init(&server, param, &server_error);
	if (server_error != TS_OK) {
		thread_server_delete(&server);
	}
	thread_server_run(&server, &server_error);
	thread_server_delete(&server);
	return NULL;
}

void thread_server_process_parent_request(thread_server *this, ts_error *err) {
	ssize_t rd;
	thread_in_head buf;
	int fd = this->poll.descriptors[this->pipe_in_idx].fd;
	rd = read(fd, &buf, sizeof(buf));
	if (rd != sizeof(buf)) {
		perror("Bad thread read");
		*err = TS_ERROR;
		return;
	}
	switch (buf.code) {
		case TH_NEW_CLIENT: thread_server_accept_client(this, err); break;
		default: *err = TS_ERROR;
	}
	return;
}

void thread_server_send_reply_parent(thread_server *this, ts_error *err) {
	pq_error poll_error;
	memory_buffer buf;
	memory_buffer to_send;
	buf.value = strdup(this->params->name);
	buf.value_length = strlen(buf.value) + 1;
	to_send.value = &buf;
	to_send.value_length = sizeof(buf);
	poll_queue_send(&this->poll, this->pipe_out_idx, &to_send, &poll_error);
	if (poll_error != PQ_OK) {
		*err = TS_ERROR;
		return;
	}
	*err = TS_OK;
	return;
}

void thread_server_process_client_request(thread_server *this, int client, ts_error *err) {
	pq_error poll_error;
	memory_buffer message;
	int fd = this->poll.descriptors[client].fd;
	char buffer[BUFFER_SIZE];
	message.value_length = strlen(this->params->name) + 1;
	message.value = this->params->name;
	if (read(fd, buffer, BUFFER_SIZE) <= 0) {
		printf("Bad read");
	}
	if (buffer[0] == '0') {
		thread_server_drop_client(this, client, err);
		*err = TS_OK;
	}  else if (buffer[0] == 'e') {
		thread_server_drop_client(this, client, err);
		*err = TS_EXIT;
	} else {
		poll_queue_send(&this->poll, client, &message, &poll_error);
		if (poll_error != PQ_OK) {
			printf("Error sending message: %d", poll_error);
			*err = TS_ERROR;
			return;
		}
		printf("Sent response to %d\n", client);
		*err = TS_OK;
	}
}

void thread_server_accept_client(thread_server *this, ts_error *err) {
	pq_error poll_error;
	thread_in_new_client data;
	int fd = this->poll.descriptors[this->pipe_in_idx].fd;
	int new_client;
	if (read(fd, &data, sizeof(data)) != sizeof(data)) {
		perror("read");
		*err = TS_ERROR;
		return;
	}
	new_client = poll_queue_insert_at(&this->poll, data.fd, CLIENTS_BEGIN, CLIENTS_END, 1, &poll_error);
	this->poll.descriptors[new_client].events = POLLIN;
	*err = TS_OK;
}

void thread_server_run(thread_server *this, ts_error *err) {
	pq_error poll_error;
	int client;
	thread_out_head head = {
		.length = 0,
		.code = TH_EXIT
	};
	memory_buffer message = {
		.value_length = sizeof(head),
		.value = &head
	};
	for(;;) {
		printf("Starting poll...\n");
		client = poll_queue_poll(&this->poll, POLL_TIMEOUT, &poll_error);
		if (poll_error != PA_OK) {
			if (errno == EINTR) {
				printf("Interrupted poll\n");
				printf("%d\n", this->poll.descriptors[this->sigint_idx].revents);
				continue;
			} else {
				printf("Error on poll: %d\n", poll_error);
				*err = TS_ERROR;
				return;
			}
		}
		
		if (client == -2) {
			printf("Poll timed out\n");
			continue;
		}
		
		if (client == this->sigint_idx) {
			printf("Bye!\n");
			*err = TS_ERROR;
			return;
		} else if (client == this->pipe_in_idx) {
			thread_server_process_parent_request(this, err);
			if (*err != TS_OK) {
				break;
			}
		} else if (client >= CLIENTS_BEGIN && client < CLIENTS_END) {
			thread_server_process_client_request(this, client, err);
			if (*err != TS_OK) {
				break;
			}
		} else {
			printf("Unprocessed client: %d\n", client);
		}
	}
	poll_queue_send(&this->poll, this->pipe_out_idx, &message, &poll_error);
	poll_queue_flush_all(&this->poll, &poll_error);
}

void thread_server_drop_client(thread_server *this, int client, ts_error *err) {
	pq_error poll_error;
	thread_out_new_client message = {
		.fd = this->poll.descriptors[client].fd
	};
	thread_out_head head = {
		.code = TH_DROP_CLIENT,
		.length = sizeof(message)
	};
	memory_buffer msgbuf = {
		.value_length = sizeof(message),
		.value = &message
	};
	memory_buffer headbuf = {
		.value_length = sizeof(head),
		.value = &head
	};
	poll_queue_flush_one(&this->poll, client, &poll_error);
	if (poll_error != PQ_OK) {
		*err = TS_ERROR;
		return;
	}
	poll_queue_send(&this->poll, this->pipe_out_idx, &headbuf, &poll_error);
	poll_queue_send(&this->poll, this->pipe_out_idx, &msgbuf, &poll_error);
	poll_queue_detach(&this->poll, client, &poll_error);
	*err = TS_OK;
	return;
}
