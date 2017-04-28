//
//  main.c
//  pseudo-client
//
//  Created by Anton Suslov on 15/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#include <stdio.h>
#include "../network/client_socket.h"
#include "../poll_array/poll_array.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define TIMEOUT -1

void ignore_signal(int signal) {
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_flags = 0;
	sigaction(signal, &act, NULL);
}

int main(int argc, const char * argv[]) {
        inet_socket client;
	nw_error net_error;
	pa_error poll_error;
	poll_array poll;
	int server_idx, stdin_idx;
	ignore_signal(SIGPIPE);
    	client_socket_init(&client, "127.0.0.1", 8080, &net_error);
        if (net_error != NW_OK) {
	    printf("Error on accept: %d\n", net_error);
	    client_socket_delete(&client);
	    return 0;
        }
	
	poll_array_init(&poll, 16, &poll_error);
	if (poll_error != PA_OK) {
		poll_array_delete(&poll);
		client_socket_delete(&client);
	}
	
	stdin_idx = poll_array_insert(&poll, STDIN_FILENO, &poll_error);
	if (poll_error != PA_OK) {
		poll_array_delete(&poll);
		client_socket_delete(&client);
	}
	poll.descriptors[stdin_idx].events = POLLIN;
	
	client_socket_release(&client);
	
	server_idx = poll_array_insert(&poll, client.sockfd, &poll_error);
	if (poll_error != PA_OK) {
		poll_array_delete(&poll);
		client_socket_delete(&client);
	}
	poll.descriptors[server_idx].events = POLLIN;
	
	char reply[BUFFER_SIZE];
	char to_send[BUFFER_SIZE];
	int reply_waiting = 0;
	while (strcmp(to_send, "stop\n")) {
		int result = poll_array_poll(&poll, TIMEOUT, &poll_error);
		
		if (result == -2) {
			printf("Poll timed out\n");
			continue;
		}
		
		if (poll_error != PA_OK) {
			printf("Error on poll %d\n", poll_error);
			client_socket_delete(&client);
			poll_array_delete(&poll);
			return 0;
		}
		
		if (poll.descriptors[result].revents & POLLHUP) {
			printf("Disconnected\n");
		}
		
		if (poll.descriptors[result].revents & POLLIN) {
			ssize_t rd;
			reply[BUFFER_SIZE - 1] = '\0';
			
			if (result == stdin_idx) {
				if (!fgets(to_send, BUFFER_SIZE, stdin)) {
					perror("Error on fgets");
					poll_array_delete(&poll);
					client_socket_delete(&client);
					return 0;
				}
				reply_waiting = 1;
				poll.descriptors[server_idx].events = POLLIN & POLLOUT;
				poll.descriptors[stdin_idx].events = 0;
				
			} else if (result == server_idx) {
				if ((rd = read(poll.descriptors[result].fd , reply, BUFFER_SIZE-1)) < 0) {
					perror("Error on read");
					poll_array_delete(&poll);
					client_socket_delete(&client);
					return 0;
				}
				printf("Reply: %s\n", reply);
				if (reply_waiting) {
					poll.descriptors[stdin_idx].events = POLLIN & POLLOUT;
				}
			}
			
		}
		
		if (poll.descriptors[result].revents & POLLOUT) {
			ssize_t wr;
			if ((wr = write(client.sockfd, to_send, BUFFER_SIZE - 1)) < 0) {
				perror("Error on write");
				poll_array_delete(&poll);
				client_socket_delete(&client);
				return 0;
			}
			reply_waiting = 0;
			poll.descriptors[server_idx].events = POLLIN;
		}
	}
	client_socket_delete(&client);
	return 0;
}
