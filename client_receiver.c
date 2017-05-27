//
// Created by ivb on 21.05.17.
//

#include "client_receiver.h"
#include "client_receiver_private.h"

#include "client_state.h"
#include "network_data.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define PROTO_VERSION 5

const char server_ip[] = "127.0.0.1";

short server_port = 51002;

void* client_receiver(void* data)
{
    int rn;
    uint32_t nplen;

    client_state* state = (client_state*)data;

    connect_to_server(state);

    while(!state->quit) {
        if((rn = read(state->sockfd, (char*)&nplen, sizeof(uint16_t))) < 0) {
            perror("ERROR: 'read' from socket failed.");
            continue;
        }
        read_packet(state, nplen);
    }

    close(state->sockfd);

    pthread_exit((void*)EXIT_SUCCESS);
}

void connect_to_server(client_state* state)
{
    if((state->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERROR: 'socket' failed.");
        pthread_exit((void*)EXIT_FAILURE);
    }

    bzero(&state->server_sin, sizeof(state->server_sin));
    state->server_sin.sin_family = AF_INET;
    state->server_sin.sin_port = htons(server_port);

    if(inet_aton(server_ip, &state->server_sin.sin_addr) == 0) {
        perror("ERROR: bad server ip.");
        close(state->sockfd);
        pthread_exit((void*)EXIT_FAILURE);
    }

    if(connect(state->sockfd, (struct sockaddr*)&state->server_sin, sizeof(state->server_sin)) < 0) {
        perror("Unable to establish connection.");
        close(state->sockfd);
        pthread_exit((void*)EXIT_FAILURE);
    }
}

void read_packet(client_state* state, uint32_t nplen)
{
    network_packet_header* header;
    char* buf;
    char* current_position;
    int i;
    int rn;

    buf = (char*)malloc(nplen);
    ((network_packet_header*)buf)->nplen = nplen;

    if((rn = read(state->sockfd, buf, nplen)) < 0) {
        perror("ERROR: 'read' from socket failed.");
        free(buf);
        return;
    }

    header = (network_packet_header*)buf;

    if(header->proto_version <= PROTO_VERSION) {
        perror("This protocol version is not supported!");
        free(buf);
        return;
    }

    current_position = buf + sizeof(header);

    for(i = 0; i < header->msg_number; ++i) {
        current_position = read_message(state, current_position);
    }

    free(buf);
}

char* read_message(client_state* state, char* msg)
{
    network_message_header* header;
    client_task* task;
    char* data;

    header = (network_message_header*) msg;

    data = msg + sizeof(network_message_header);

    task = (client_task*) malloc(sizeof(client_task));
    task->data = malloc(header->msglen - sizeof(network_message_header));
    task->task_type = SERVER_RESPONSE;
    task->data_type.net_type = header->type;

    strcpy(task->data, data);

    client_task_queue_push(&state->queue, task);

    return msg + header->msglen;
}