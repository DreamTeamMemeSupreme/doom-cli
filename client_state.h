//
// Created by snixosha on 30.04.17.
//

#ifndef DOOM_CLIENT_STATE_H
#define DOOM_CLIENT_STATE_H

#include <stdbool.h>
#include <stdatomic.h>
#include "network_data.h"
#include "client_task_queue.h"
#include <pthread.h>
#include <netinet/in.h>

typedef enum {
    DEFAULT,
    INFO,
    LIST,
    GAME
} stage_type;

typedef struct {
    stage_type stage;
    input_type input;
    bool connection;
    atomic_int quit;
    uint16_t page;
    response_team_info_data* team_info;
	response_team_list_data* team_list;
	char name[30];
	int sockfd;
	struct sockaddr_in server_sin;
    pthread_t user_input;
    pthread_t server_receive;
    struct client_task_queue queue;
} client_state;

#endif //DOOM_CLIENT_STATE_H
