//
//  server-PRIVATE.h
//  doom-cli
//
//  Created by Anton Suslov on 26/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef server_PRIVATE_h
#define server_PRIVATE_h

#include "server.h"

void ps_accept_conn(ps_server *this, int client, ps_error *err);

void ps_process_client(ps_server *this, int client, ps_error *err);

void ps_client_read(ps_server *this, int client, ps_error *err);

void ps_client_write(ps_server *this, int client, ps_error *err);

void ps_move_client(ps_server *this, int client, int room, ps_error *err);

void ps_create_thread(ps_server *this, int client, ps_error *err);

void ps_process_thread_reply(ps_server *this, int client, ps_error *err);

void ps_accept_thread_client(ps_server *this, int client, ps_error *err);

void ps_process_thread_stop(ps_server *this, int client, ps_error *err);

#endif /* server_PRIVATE_h */
