//
//  socket_client.h
//  doom-cli
//
//  Created by Anton Suslov on 18/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef socket_client_h
#define socket_client_h

#include <sys/types.h>
#include "socket.h"

//Initializes client socket and connects to given host.
//this: pointer to the socket
//host: IP adress of a remote host
//port: remote port
//err: error code
void client_socket_init(inet_socket *this, const char *host, int port, nw_error *err);

void client_socket_delete(inet_socket *this);

//After invoking this function,
//delete call will not close the socket,
//it would need to be closed manually.
void client_socket_release(inet_socket *this);

#endif /* socket_client_h */
