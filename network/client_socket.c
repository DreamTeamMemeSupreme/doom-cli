//
//  client_socket.c
//  doom-cli
//
//  Created by Anton Suslov on 18/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client_socket.h"

void client_socket_init(inet_socket *this, const char *host, int port, nw_error *err) {
	this->active = 0;
	struct hostent *resolved_host = gethostbyname(host);
	if (!resolved_host) {
		perror("Error resolving host");
		*err = NW_HOSTNAME_ERR;
		return;
	}
	if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		*err = NW_SOCKET_CREATE_ERR;
		return;
	}
	memset(&this->serv_addr, 0 , sizeof(this->serv_addr));
	this->serv_addr.sin_addr.s_addr = inet_addr(host);
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_port = htons(port);
	if (connect(this->sockfd, (struct sockaddr *) &this->serv_addr, sizeof(this->serv_addr)) < 0) {
		perror("Connect error");
		*err = NW_ACCEPT_ERR;
		close(this->sockfd);
		return;
	}
	this->active = 1;
	*err = NW_OK;
}

void client_socket_delete(inet_socket *this) {
	if (this->active) {
		close(this->sockfd);
	}
}

void client_socket_release(inet_socket *this) {
	this->active = 0;
}
