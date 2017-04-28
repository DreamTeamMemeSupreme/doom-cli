//
//  network.c
//  doom-cli
//
//  Created by Anton Suslov on 15/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include <arpa/inet.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server_socket.h"


void server_socket_init(inet_socket *this, int port, int backlog, nw_error *err) {
	this->active = 0;
	if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		*err = NW_ACCEPT_ERR;
		return;
	}
	memset(&this->serv_addr, 0 , sizeof(this->serv_addr));
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;
	this->serv_addr.sin_port = htons(port);
	if (bind(this->sockfd, (struct sockaddr *) &this->serv_addr, sizeof(this->serv_addr)) < 0) {
		perror("Bind error");
		*err = NW_BIND_ERR;
		close(this->sockfd);
		return;
	}
	if (listen(this->sockfd, backlog) < 0) {
		*err = NW_LISTEN_ERR;
	}
	this->active = 1;
	*err = NW_OK;
}

void server_socket_delete(inet_socket *this) {
	if (this->active) {
		close(this->sockfd);
		this->active = 0;
	}
}

void server_socket_accept(inet_socket *this, nw_error *err, inet_socket *output) {
	int new_fd;
	socklen_t addr_size = sizeof(output->serv_addr);
	new_fd = accept(this->sockfd, (struct sockaddr *) &output->serv_addr, &addr_size);
	if (new_fd > 0) {
		output->sockfd = new_fd;
	} else {
		perror("Error on accept");
		*err = NW_ACCEPT_ERR;
		return;
	}
	*err = NW_OK;
}

void server_socket_release(inet_socket *this) {
	this->active = 0;
}
