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
#include <pthread.h>
#include "../poll_queue/poll_queue.h"
#include "../network/socket.h"

#define THREAD_COUNT 1024
#define RESERVED 4
#define RESERVED_BEGIN 0
#define RESERVED_END (RESERVED_BEGIN + RESERVED)
#define CLIENTS 16
#define CLIENTS_BEGIN RESERVED_END
#define CLIENTS_END (CLIENTS_BEGIN + CLIENTS)
#define THREADS 4
#define THREADS_IN_BEGIN (CLIENTS_END)
#define THREADS_IN_END (THREADS_IN_BEGIN + THREAD_COUNT)
#define THREADS_OUT_BEGIN (THREADS_IN_END)
#define THREADS_OUT_END (THREADS_OUT_BEGIN + THREAD_COUNT)
#define POLL_SIZE (THREADS_OUT_END)
#define BUFFER_SIZE 4096
#define POLL_TIMEOUT -1
#define BACKLOG 16

extern int sigint_pipe[2];

typedef enum {
	PS_OK = 0,
	PS_EXIT,
	PS_ERROR
} ps_error;

typedef struct {
	poll_queue poll;
	int server_idx;
	int sigint_idx;
	int stdin_idx;
	inet_socket server;
	char reply[BUFFER_SIZE];
	int thread_pipes[THREAD_COUNT][2]; //1 is for input, 0 for output
	pthread_t threads[THREAD_COUNT];
	int thread_count;
} ps_server;

void ps_init(ps_server *this, ps_error *err);

void ps_run(ps_server *this, ps_error *err);

void ps_delete(ps_server *this);

#endif /* server_h */
