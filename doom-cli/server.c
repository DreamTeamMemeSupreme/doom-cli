//
//  server.c
//  doom-cli
//
//  Created by Anton Suslov on 15/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include "server.h"

void server_init(game_server *this, int port) {
	this->port = port;
	this->backlog = 16;
}

void server_run(game_server *this, server_error *err) {
	nw_error socket_error;
	server_socket_init(&this->socket, this->port, &socket_error);
	if (socket_error != NETWORK_OK) {
		*err = SERVER_CRASH;
		return;
	}
	server_socket_listen(&this->socket, this->backlog, &socket_error);
	if (socket_error != NETWORK_OK) {
		*err = SERVER_CRASH;
		server_socket_delete(&this->socket);
		return;
	}
	while(1) {
		int client = server_socket_poll(&this->socket, SERVER_TIMEOUT, &socket_error);
		if (socket_error != NETWORK_OK) {
			*err = SERVER_CRASH;
			server_socket_delete(&this->socket);
			return;
		}
		if (client == 0) {
			server_socket_accept(&this->socket, &socket_error);
			if (socket_error != NETWORK_OK) {
				*err = SERVER_CRASH;
				server_socket_delete(&this->socket);
				return;
			}
		} else {
			uint32_t message_length;
			server_socket_read(&this->socket, client, &message_length, sizeof(message_length), &socket_error);
			if (socket_error != NETWORK_OK) {
				*err = SERVER_CRASH;
				server_socket_delete(&this->socket);
				return;
			}
			for (; message_length; message_length--) {
				uint16_t packet_size;
				uint16_t packet_type;
				server_socket_read(&this->socket, client, &packet_type, sizeof(packet_type), &socket_error);
			}
		}
	}
}
