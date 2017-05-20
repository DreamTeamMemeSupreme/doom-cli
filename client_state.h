//
// Created by snixosha on 30.04.17.
//

#ifndef DOOM_CLIENT_STATE_H
#define DOOM_CLIENT_STATE_H

#include <stdbool.h>
#include "network_data.h"
#include "client_task_queue.h"
#include <pthread.h>

typedef enum {
    DEFAULT,
    INFO,
    LOBBY,
    LIST,
    GAME
} stage_type;

typedef struct {
    stage_type stage;
    input_type input;
    bool connection;
    bool quit;
    uint16_t page;
    uint16_t max_page;
    response_team_info_data* team_info;
    pthread_t user_input;
    pthread_t server_receive;
    struct client_task_queue queue;
} client_state;

#endif //DOOM_CLIENT_STATE_H
