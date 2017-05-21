//
//  server.c
//  doom-cli
//
//  Created by Anton Suslov on 26/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../network/server_socket.h"
#include "server.h"
#include "server-PRIVATE.h"
#include "thread.h"

void ps_accept_conn(ps_server *this, int client, ps_error *err) {
	pq_error poll_error;
	int accepted;
	accepted = accept(this->poll.descriptors[client].fd, NULL, 0);
	if (accepted < 0) {
		perror("Error on accept");
		*err = PS_ERROR;
		return;
	}
	accepted = poll_queue_insert_at(&this->poll, accepted,
					CLIENTS_BEGIN, CLIENTS_END, 1,
					&poll_error);
	if (poll_error != PA_OK) {
		printf("Error on adding new client: %d\n", poll_error);
		*err = PS_ERROR;
		return;
	}
	this->poll.descriptors[accepted].events = POLLIN;
	printf("Client %d connected\n", accepted);
	*err = PS_OK;
}

void ps_init(ps_server *this, ps_error *err) {
	nw_error net_error;
	pq_error poll_error;
	memset(this, 0, sizeof(*this));
	server_socket_init(&this->server, 8080, BACKLOG, &net_error);
	if (net_error != NW_OK) {
		printf("Error on init: %d\n", net_error);
		*err = PS_ERROR;
		return;
	}
	
	poll_queue_init(&this->poll, POLL_SIZE, &poll_error);
	if (poll_error != PQ_OK) {
		printf("Error on poll init: %d\n", poll_error);
		*err = PS_ERROR;
		return;
	}
	
	this->server_idx = poll_queue_insert_at(&this->poll, this->server.sockfd,
						RESERVED_BEGIN, RESERVED_BEGIN + RESERVED, 0,
						&poll_error);
	if (poll_error != PQ_OK) {
		printf("Error on adding server socket: %d\n", poll_error);
		*err = PS_ERROR;
		return;
	}
	//server_socket_release(&server);
	this->poll.descriptors[this->server_idx].events = POLLIN;
	
	this->stdin_idx = poll_queue_insert_at(&this->poll, STDIN_FILENO,
					       RESERVED_BEGIN, RESERVED_BEGIN + RESERVED, 1,
					       &poll_error);
	if (poll_error != PQ_OK) {
		printf("Error on adding stdin: %d\n", poll_error);
		*err = PS_ERROR;
		return;
	}
	this->poll.descriptors[this->stdin_idx].events = POLLIN;
	
	this->sigint_idx = poll_queue_insert_at(&this->poll, sigint_pipe[0],
						RESERVED_BEGIN, RESERVED_BEGIN + RESERVED, 0,
						&poll_error);
	if (poll_error != PQ_OK) {
		printf("Error on adding sigint pipe: %d\n", poll_error);
		*err = PS_ERROR;
		return;
	}
	printf("Registered sigint, client id: %d\n", sigint_pipe[0]);
	this->poll.descriptors[this->sigint_idx].events = POLLIN;
}

void ps_run(ps_server *this, ps_error *err) {
	int client;
	pq_error poll_error;
	for(;;) {
		printf("Starting poll...\n");
		client = poll_queue_poll(&this->poll, POLL_TIMEOUT, &poll_error);
		if (poll_error != PA_OK) {
			if (errno == EINTR) {
				printf("Interrupted poll\n");
				printf("%d\n", this->poll.descriptors[this->sigint_idx].revents);
				break;
			} else {
				printf("Error on poll: %d\n", poll_error);
				*err = PS_ERROR;
				break;
			}
		}
		
		if (client == -2) {
			printf("Poll timed out\n");
			continue;
		}
		
		if (client == this->sigint_idx) {
			printf("Bye!\n");
			*err = PS_OK;
			break;
		} else if (client == this->server_idx) {
			ps_accept_conn(this, client, err);
			if (*err != PS_OK) {
				break;
			}
			
		} else if (client == this->stdin_idx) {
			fgets(this->reply, BUFFER_SIZE, stdin);
			if (feof(stdin)) {
				*err = PS_OK;
				break;
			}
			printf("New reply saved\n");
		} else if (client >= CLIENTS_BEGIN && client < CLIENTS_BEGIN + CLIENTS) {
			ps_process_client(this, client, err);
			if (*err != PS_OK) {
				break;
			}
		} else if (client >= THREADS_IN_BEGIN && client < THREADS_IN_END) {
			ps_process_thread_reply(this, client, err);
		} else {
			printf("Unprocessed client: %d\n", client);
		}
	}
	poll_queue_flush_all(&this->poll, &poll_error);
}

void ps_process_client(ps_server *this, int client, ps_error *err) {
	pq_error poll_error;
	short event = this->poll.descriptors[client].revents;
	printf("Client %d status: %d\n", client, event);
	
	if (event & POLLHUP) {
		poll_queue_close(&this->poll, client, &poll_error);
		if (poll_error != PA_OK) {
			printf("Error on disconnecting: %d\n", poll_error);
			*err = PS_ERROR;
			return;
		}
		printf("Client %d dropped connection\n", client);
		
	} else if (event & POLLIN) {
		ps_client_read(this, client, err);
		if (*err != PS_OK) {
			return;
		}
	}
	
}

void ps_client_read(ps_server *this, int client, ps_error *err) {
	char message[BUFFER_SIZE];
	ssize_t rd;
	int room;
	rd = read(this->poll.descriptors[client].fd, message, BUFFER_SIZE - 1);
	if (rd < 0) {
		perror("Error on read");
		*err = PS_ERROR;
		return;
	}
	if (message[rd - 1] == '\n') {
		message[rd - 1] = '\0';
	}
	if (!rd) {
		printf("Empty read\n");
		*err = PS_ERROR;
		return;
	}
	if (rd) {
		printf("%d: %s\n", client, message);
	}
	
	if (isdigit(message[0])) {
		room = message[0] - '0';
		if (!this->threads[room]) {
			ps_create_thread(this, room, err);
			if (*err != PS_OK) {
				return;
			}
		}
		ps_move_client(this, client, room, err);
	} else if (message[0] == 'q') {
		*err = PS_EXIT;
	} else {
		this->poll.descriptors[client].events = POLLIN;
		ps_client_write(this, client, err);
	}
}

void ps_client_write(ps_server *this, int client, ps_error *err) {
	pq_error poll_error;
	memory_buffer *message = malloc(sizeof(*message));
	message->value_length = strlen(this->reply) + 1;
	message->value = strdup(this->reply);
	poll_queue_send(&this->poll, client, message, &poll_error);
	if (poll_error != PQ_OK) {
		printf("Error sending message: %d", poll_error);
		*err = PS_ERROR;
		return;
	}
	printf("Sent response to %d\n", client);
	free(message->value);
	free(message);
	*err = PS_OK;
}

void ps_delete(ps_server *this) {
	poll_queue_delete(&this->poll);
	server_socket_delete(&this->server);
}

void ps_move_client(ps_server *this, int client, int room, ps_error *err) {
	pq_error poll_error;
	memory_buffer message_head;
	memory_buffer message_body;
	thread_in_new_client value_body = {
		.fd = this->poll.descriptors[client].fd
	};
	thread_in_head value_head = {
		.length = sizeof(value_body),
		.code = TH_NEW_CLIENT
	};
	message_head.value_length = sizeof(value_head);
	message_head.value = &value_head;
	message_body.value_length = sizeof(value_body);
	message_body.value = &value_body;
	if (poll_queue_detach(&this->poll, client, &poll_error) < 0) {
		*err = PS_ERROR;
	}
	poll_queue_send(&this->poll, THREADS_OUT_BEGIN + room, &message_head, &poll_error);
	if (poll_error != PQ_OK) {
		*err = PS_ERROR;
	}
	poll_queue_send(&this->poll, THREADS_OUT_BEGIN + room, &message_body, &poll_error);
	if (poll_error != PQ_OK) {
		*err = PS_ERROR;
	}
	*err = PS_OK;
}

void ps_accept_thread_client(ps_server *this, int client, ps_error *err) {
	thread_out_new_client message;
	pq_error poll_error;
	int new_client;
	int fd = this->poll.descriptors[client].fd;
	if (read(fd, &message, sizeof(message)) < 0) {
		*err = PS_ERROR;
		return;
	}
	new_client = poll_queue_insert_at(&this->poll, message.fd, CLIENTS_BEGIN, CLIENTS_END, 1, &poll_error);
	if (poll_error != PQ_ERROR) {
		*err = PS_ERROR;
	}
	this->poll.descriptors[new_client].events = POLLIN;
	*err = PS_OK;
}

void ps_process_thread_reply(ps_server *this, int client, ps_error *err) {
	thread_out_head head;
	int code = this->poll.descriptors[client].revents;
	ssize_t rd;
	if (code & POLLIN) {
		int fd = this->poll.descriptors[client].fd;
		if ((rd = read(fd, &head, sizeof(head))) < 0) {
			*err = PS_ERROR;
			return;
		}
		if (rd == 0) {
			ps_process_thread_stop(this, client, err);
		} else {
			switch (head.code) {
				case TH_DROP_CLIENT: ps_accept_thread_client(this, client, err); break;
				case TH_EXIT: ps_process_thread_stop(this, client, err);
			}
		}
	}
}

void ps_process_thread_stop(ps_server *this, int client, ps_error *err) {
	int thread = client - THREADS_IN_BEGIN;
	this->poll.descriptors[client].events = 0;
	printf("Thread %d stopped\n", thread);
}

void ps_create_thread(ps_server *this, int room, ps_error *err) {
	pq_error poll_error;
	thread_params *param = malloc(sizeof(*param));
	int pipe_to_thread[2];
	int pipe_from_thread[2];
	if (pipe(pipe_to_thread) < 0) {
		perror("Pipe to thread:");
		*err = PS_ERROR;
	}
	if (pipe(pipe_from_thread) < 0) {
		perror("Pipe from thread:");
		close(pipe_to_thread[0]);
		close(pipe_to_thread[1]);
		*err = PS_ERROR;
	}
	param->name = strdup("server");
	param->name[0] = room % 10 + '0';
	param->pipe_in = pipe_to_thread[0];
	param->pipe_out = pipe_from_thread[1];
	pthread_create(&this->threads[room], NULL, thread_server_f, param);
	poll_queue_insert(&this->poll, pipe_from_thread[0], THREADS_IN_BEGIN + room, 1, &poll_error);
	if (poll_error != PQ_OK) {
		*err = PS_ERROR;
	}
	this->poll.descriptors[THREADS_IN_BEGIN + room].events = POLLIN;
	poll_queue_insert(&this->poll, pipe_to_thread[1], THREADS_OUT_BEGIN + room, 1, &poll_error);
	if (poll_error != PS_OK) {
		*err = PS_ERROR;
	}
	*err = PS_OK;
}
