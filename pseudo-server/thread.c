//
//  thead.c
//  doom-cli
//
//  Created by Anton Suslov on 27/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "thread-PRIVATE.h"
#include "thread.h"
#include <errno.h>


void thread_server_init(thread_server *this, thread_params *args, ts_error *err) {
	pa_error poll_error;
	memset(this, 0, sizeof(*this));
	poll_array_init(&this->poll, POLL_SIZE, &poll_error);
	this->params.name = strdup(args->name);
	this->params.pipe_in = args->pipe_in;
	this->params.pipe_out = args->pipe_out;
	this->pipe_in_idx = poll_array_insert_at(&this->poll, this->params.pipe_in,
						 RESERVED_BEGIN, RESERVED_END, &poll_error);
	if (poll_error != PA_OK) {
		*err = TS_ERROR;
		return;
	}
	
	this->pipe_out_idx = poll_array_insert_at(&this->poll, this->params.pipe_out,
						 RESERVED_BEGIN, RESERVED_END, &poll_error);
	if (poll_error != PA_OK) {
		*err = TS_ERROR;
		return;
	}
	
	this->sigint_idx = poll_array_insert_at(&this->poll, sigint_pipe[0],
						  RESERVED_BEGIN, RESERVED_END, &poll_error);
	if (poll_error != PA_OK) {
		*err = TS_ERROR;
		return;
	}
}

void thread_server_delete(thread_server *this) {
	if (this->params.name) {
		free(this->params.name);
	}
	close(this->params.pipe_in);
	close(this->params.pipe_out);
}

void *thread_server_f(void *param) {
	ts_error server_error;
	thread_server server;
	memset(&server, 0, sizeof(server));
	thread_server_init(&server, param, &server_error);
	free(param);
	if (server_error != TS_OK) {
		thread_server_delete(&server);
	}
	thread_server_run(&server, &server_error);
	thread_server_delete(&server);
	return NULL;
}

void thread_server_run(thread_server *this, ts_error *err) {
	pa_error poll_error;
	int client;
	for(;;) {
		printf("Starting poll...\n");
		client = poll_array_poll(&this->poll, POLL_TIMEOUT, &poll_error);
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
			ps_accept_conn(this, client, err);
			if (*err != TS_OK) {
				return;
			}
		} else if (client >= CLIENTS_BEGIN && client < CLIENTS_END) {
			ps_process_client(this, client, err);
			if (*err != PS_OK) {
				return;
			}
		} else {
			printf("Unprocessed client: %d\n", client);
		}
	}
}
