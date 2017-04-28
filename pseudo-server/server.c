//
//  server.c
//  doom-cli
//
//  Created by Anton Suslov on 26/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include "server.h"
#include "signal.h"
#include "stdio.h"
#include "unistd.h"
#include "../network/server_socket.h"
#include "string.h"
#include "errno.h"
#include "server-PRIVATE.h"

void ps_accept_conn(ps_server *this, int client, ps_error *err) {
	pa_error poll_error;
	int accepted;
	accepted = accept(this->poll.descriptors[client].fd, NULL, 0);
	if (accepted < 0) {
		perror("Error on accept");
		*err = PS_ERROR;
		return;
	}
	accepted = poll_array_insert_at(&this->poll, accepted,
					CLIENTS_BEGIN, CLIENTS_BEGIN + CLIENTS, 1,
					&poll_error);
	if (poll_error != PA_OK) {
		printf("Error on adding new client: %d\n", poll_error);
		*err = PS_ERROR;
		return;
	}
	this->poll.descriptors[accepted].events = POLLIN;
	printf("Client %d connected\n", accepted);
}

void ps_init(ps_server *this, ps_error *err) {
	nw_error net_error;
	pa_error poll_error;
	inet_socket server;
	memset(this, 0, sizeof(*this));
	server_socket_init(&server, 8080, BACKLOG, &net_error);
	if (net_error != NW_OK) {
		printf("Error on init: %d\n", net_error);
		server_socket_delete(&server);
		*err = PS_ERROR;
		return;
	}
	
	poll_array_init(&this->poll, POLL_SIZE, &poll_error);
	if (poll_error != PA_OK) {
		printf("Error on poll init: %d\n", poll_error);
		server_socket_delete(&server);
		*err = PS_ERROR;
		return;
	}
	
	this->server_idx = poll_array_insert_at(&this->poll, server.sockfd,
						RESERVED_BEGIN, RESERVED_BEGIN + RESERVED, 1,
						&poll_error);
	if (poll_error != PA_OK) {
		printf("Error on adding server socket: %d\n", poll_error);
		server_socket_delete(&server);
		*err = PS_ERROR;
		return;
	}
	server_socket_release(&server);
	this->poll.descriptors[this->server_idx].events = POLLIN;
	
	this->stdin_idx = poll_array_insert_at(&this->poll, STDIN_FILENO,
					       RESERVED_BEGIN, RESERVED_BEGIN + RESERVED, 1,
					       &poll_error);
	if (poll_error != PA_OK) {
		printf("Error on adding stdin: %d\n", poll_error);
		*err = PS_ERROR;
		return;
	}
	this->poll.descriptors[this->stdin_idx].events = POLLIN;
	
	this->sigint_idx = poll_array_insert_at(&this->poll, sigint_pipe[0],
						RESERVED_BEGIN, RESERVED_BEGIN + RESERVED, 0,
						&poll_error);
	if (poll_error != PA_OK) {
		printf("Error on adding sigint pipe: %d\n", poll_error);
		*err = PS_ERROR;
		return;
	}
	printf("Registered sigint, client id: %d\n", sigint_pipe[0]);
	this->poll.descriptors[this->sigint_idx].events = POLLIN;
	
	server_socket_delete(&server);
}

void ps_run(ps_server *this, ps_error *err) {
	int client;
	pa_error poll_error;
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
				*err = PS_ERROR;
				return;
			}
		}
		
		if (client == -2) {
			printf("Poll timed out\n");
			continue;
		}
		
		if (client == this->sigint_idx) {
			printf("Bye!\n");
			*err = PS_OK;
			return;
		} else if (client == this->server_idx) {
			ps_accept_conn(this, client, err);
			if (*err != PS_OK) {
				return;
			}
			
		} else if (client == this->stdin_idx) {
			fgets(this->reply, BUFFER_SIZE, stdin);
			printf("New reply saved\n");
		} else if (client >= CLIENTS_BEGIN && client < CLIENTS_BEGIN + CLIENTS) {
			ps_process_client(this, client, err);
			if (*err != PS_OK) {
				return;
			}
		} else {
			printf("Unprocessed client: %d\n", client);
		}
	}
}

void ps_process_client(ps_server *this, int client, ps_error *err) {
	pa_error poll_error;
	short event = this->poll.descriptors[client].revents;
	printf("Client %d status: %d\n", client, event);
	
	if (event & POLLOUT) {
		ps_client_write(this, client, err);
		if (*err != PS_OK) {
			return;
		}
	}
	
	if (event & POLLHUP) {
		poll_array_close(&this->poll, client, &poll_error);
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
	rd = read(this->poll.descriptors[client].fd, message, BUFFER_SIZE - 1);
	if (rd < 0) {
		perror("Error on read");
		*err = PS_ERROR;
		return;
	}
	if (message[rd - 1] == '\n') {
		message[rd - 1] = '\0';
	}
	if (rd) {
		printf("%d: %s\n", client, message);
	}
	this->poll.descriptors[client].events = POLLIN | POLLOUT;
}

void ps_client_write(ps_server *this, int client, ps_error *err) {
	ssize_t wr;
	wr = write(this->poll.descriptors[client].fd, this->reply, strlen(this->reply));
	if (wr < 0) {
		perror("Write to client failed");
		*err = PS_ERROR;
		return;
	}
	printf("Sent response to %d\n", client);
	this->poll.descriptors[client].events = POLLIN;
}

void ps_delete(ps_server *this) {
	poll_array_delete(&this->poll);
}
