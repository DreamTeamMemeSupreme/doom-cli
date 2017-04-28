//
//  server.h
//  doom-cli
//
//  Created by Anton Suslov on 26/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef server_h
#define server_h

#include <stdio.h>
#include "../poll_array/poll_array.h"

#define THREAD_COUNT 1024
#define RESERVED 4
#define RESERVED_BEGIN 0
#define CLIENTS 16
#define CLIENTS_BEGIN RESERVED_BEGIN + RESERVED
#define THREADS 4
#define THREADS_IN_BEGIN CLIENTS_BEGIN + CLIENTS
#define THREADS_OUT_BEGIN THREADS_IN_BEGIN + THREADS
#define POLL_SIZE THREADS_OUT_BEGIN + THREADS
#define BUFFER_SIZE 4096
#define POLL_TIMEOUT -1
#define BACKLOG 16

extern int sigint_pipe[2];

typedef enum {
	PS_OK = 0,
	PS_ERROR
} ps_error;

typedef struct {
	poll_array poll;
	int server_idx;
	int sigint_idx;
	int stdin_idx;
	char reply[BUFFER_SIZE];
	int thread_pipes[THREADS][2];
} ps_server;

void ps_init(ps_server *this, ps_error *err);

void ps_run(ps_server *this, ps_error *err);

void ps_delete(ps_server *this);

#endif /* server_h */
