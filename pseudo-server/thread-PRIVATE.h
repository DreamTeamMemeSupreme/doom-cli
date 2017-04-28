//
//  thread-PRIVATE.h
//  doom-cli
//
//  Created by Anton Suslov on 27/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef thread_PRIVATE_h
#define thread_PRIVATE_h

#include "thread.h"

#define RESERVED 24
#define RESERVED_BEGIN 0
#define RESERVED_END RESERVED_BEGIN + RESERVED
#define CLIENTS 1000
#define CLIENTS_BEGIN RESERVED_END
#define CLIENTS_END CLIENTS_BEGIN + CLIENTS
#define POLL_SIZE CLIENTS_END

typedef enum {
	TS_OK,
	TS_ERROR
} ts_error;

typedef struct {
	poll_array poll;
	thread_params params;
	int pipe_in_idx;
	int pipe_out_idx;
	int sigint_idx;
} thread_server;

void thread_server_init(thread_server *this, thread_params *args, ts_error *err);

void thread_server_delete(thread_server *this);

void thread_server_process_parent_request(thread_server *this, ts_error *err);

void thread_server_send_parent_reply(thread_server *this, ts_error *err);

void thread_server_run(thread_server *this, ts_error *err);

#endif /* thread_PRIVATE_h */
