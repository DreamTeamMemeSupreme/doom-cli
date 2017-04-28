//
//  network.h
//  doom-cli
//
//  Created by Anton Suslov on 15/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef network_h
#define network_h

#include <sys/types.h>
#include "socket.h"

//Initializes server socket with given port and starts listening
//this: pointer to the socket
//port: which port the socket should be bound to
//backlog: how many connections can wait for accept
//err: error code
void server_socket_init(inet_socket *this, int port, int backlog, nw_error *err);

//Deletes server socket, which stops accepting connections.
//Existing connections are not terminated.
//Once deleted, it should be re-initialized to work.
//this: pointer to the socket.
void server_socket_delete(inet_socket *this);

//Accept new connection to the socket
//this: pointer to the socket
//err: error code
//returns: corresponding index in conns for new client
void server_socket_accept(inet_socket *this, nw_error *err, inet_socket *output);

void server_socket_release(inet_socket *this);
#endif /* network_h */
