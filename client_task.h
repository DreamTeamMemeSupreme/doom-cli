//
// Created by snixosha on 30.04.17.
//

#ifndef DOOM_CLIENT_TASK_H
#define DOOM_CLIENT_TASK_H

#include "network_data.h"
#include <stdlib.h>

typedef enum {
    SERVER_RESPONSE,
    USER_INPUT
} task_type;

typedef enum {
    COMMAND,
    KEY_PRESSED
} input_type;

typedef union {
    network_packet net_type;
    input_type input_type;
} data_type;

typedef struct {
    task_type task_type;
    data_type data_type;
    void* data;
} client_task;

void client_task_delete( client_task* task );

#endif //DOOM_CLIENT_TASK_H
