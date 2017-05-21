//
//  thead.h
//  doom-cli
//
//  Created by Anton Suslov on 27/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef thead_h
#define thead_h

#include <stdio.h>

#define POLL_TIMEOUT -1

extern int sigint_pipe[2];

typedef enum {
	TH_EXIT = 0,
	TH_DROP_CLIENT
} thread_out;

typedef enum {
	TH_NEW_CLIENT
} thread_in;

typedef struct {
    size_t length;
    thread_in code;
} thread_in_head;

typedef struct {
	size_t length;
	thread_out code;
} thread_out_head;

typedef struct {
	int fd;
} thread_in_new_client;

typedef struct {
    int fd;
} thread_out_new_client;

typedef struct {
	char *name;
	int pipe_in;
	int pipe_out;
} thread_params;

typedef struct {
	int client;
} thread_msg;

void *thread_server_f(void *param);

#endif /* thead_h */
