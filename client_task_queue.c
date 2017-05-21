//
// Created by snixosha on 30.04.17.
//

#include "client_task_queue.h"
#include "client_task.h"

void client_task_queue_push( struct client_task_queue* queue, client_task* task ) {

    pthread_mutex_lock(&(queue->m));

    queue->tail = malloc(sizeof(struct client_task_queue_node));
    queue->tail->task = task;
    queue->tail->prev = 0;
    if( queue->head == 0 )
        queue->head = queue->tail;

    queue->tail = 0;
    pthread_mutex_unlock(&(queue->m));

};

client_task* client_task_queue_pop( struct client_task_queue* queue ) {
    pthread_mutex_lock(&(queue->m));

    if( queue->head == 0 ) {
        pthread_mutex_unlock(&(queue->m));
        return 0;
    }

    client_task* res = queue->head->task;
    queue->head = queue->head->prev;

    pthread_mutex_unlock(&(queue->m));
    return res;
}

void client_task_queue_init( struct client_task_queue* queue ) {
    queue->tail = 0;
    queue->head = 0;
    pthread_mutex_init(&(queue->m),NULL);
}

void client_task_queue_delete( struct client_task_queue* queue ) {
    pthread_mutex_destroy(&queue->m);
    struct client_task_queue_node* node = queue->head;
    while( node != 0 ) {
        client_task_delete(node->task);
        node = node->prev;
    }
}
