//
//  socket.h
//  doom-cli
//
//  Created by Anton Suslov on 18/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef socket_h
#define socket_h

#define SOCKET_MAX_CONNS 1023

#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>

typedef enum {
	NW_OK = 0,
	NW_UNKNOWN,
	NW_ACCEPT_ERR,
	NW_SOCKET_CREATE_ERR,
	NW_BIND_ERR,
	NW_LISTEN_ERR,
	NW_HOSTNAME_ERR
} nw_error;

typedef struct {
	int sockfd;
	char active;
	struct sockaddr_in serv_addr;
} inet_socket;

#endif /* socket_h */
