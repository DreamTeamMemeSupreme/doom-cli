//
//  main.c
//  pseudo-server
//
//  Created by Anton Suslov on 17/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include <stdio.h>
#include <poll.h>
#include "server.h"
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <errno.h>
#include <pthread.h>

int sigint_pipe[2] = {-1};

typedef struct {
	poll_array poll;
	
} thread_server;

typedef struct {
	int fd;
	void *request;
} client_transfer;

void *threaded_task(void *data) {
	return NULL;
}

void ignore_signal(int signal) {
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_flags = 0;
	sigaction(signal, &act, NULL);
}

void process_sigint(int signal) {
	printf("call\n");
	if (signal == SIGINT) {
		if (sigint_pipe[1] < 0) {
			exit(-1);
		} else {
			ssize_t wr;
			if ((wr = write(sigint_pipe[1], "1", 2)) < 0) {
				perror(NULL);
				exit(-1);
			}
			//printf("%zd\n", wr);
		}
	}
}

int set_fo_limit(int limit) {
	struct rlimit lim;
	if (getrlimit(RLIMIT_NOFILE, &lim) < 0) {
		perror("Error getting limit");
		return -1;
	}
	lim.rlim_cur = limit;
	if (setrlimit(RLIMIT_NOFILE, &lim) < 0) {
		perror("Error setting limit");
		return -1;
	}
	return 0;
}

int main(int argc, const char * argv[]) {
	ps_server server;
	ps_error error;
	set_fo_limit(8096);
	signal(SIGINT, process_sigint);
	if (pipe(sigint_pipe) < 0) {
		perror("Pipe failed");
		return -1;
	}
	ps_init(&server, &error);
	if (error != PS_OK) {
		ps_delete(&server);
		return -1;
	}
	ps_run(&server, &error);
	ps_delete(&server);
	close(sigint_pipe[0]);
	close(sigint_pipe[1]);
	if (error == PS_OK) {
		return 0;
	} else {
		return -1;
	}
}

