//
// Created by snixosha on 30.04.17.
//

#ifndef DOOM_CLIENT_TASK_QUEUE_H
#define DOOM_CLIENT_TASK_QUEUE_H

#include "client_task.h"
#include <pthread.h>

struct client_task_queue_node {
    struct client_task_queue_node* prev;
    client_task* task;
};

struct client_task_queue {
    struct client_task_queue_node* head;
    struct client_task_queue_node* tail;
    pthread_mutex_t m;
};

void client_task_queue_push( struct client_task_queue* queue, client_task* task );

client_task* client_task_queue_pop( struct client_task_queue* queue );

void client_task_queue_init( struct client_task_queue* queue );

void client_task_queue_delete( struct client_task_queue* queue );

#endif //DOOM_CLIENT_TASK_QUEUE_H
