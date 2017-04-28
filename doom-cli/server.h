//
//  server.h
//  doom-cli
//
//  Created by Anton Suslov on 15/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef server_h
#define server_h

#include "../network/server_socket.h"
#include <errno.h>
#include <stdio.h>

#define SERVER_TIMEOUT -1

typedef enum {
	SERVER_CRASH,
	SERVER_OK
} server_error;

typedef struct {
	int port;
	int backlog;
	inet_socket socket;
} game_server;

void server_init(game_server *this, int port);

void server_run(game_server *this, server_error *err);

#endif /* server_h */
