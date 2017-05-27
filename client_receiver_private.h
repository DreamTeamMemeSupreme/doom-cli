//
// Created by ivb on 21.05.17.
//

#ifndef DOOM_CLIENT_RECEIVER_PRIVATE_H
#define DOOM_CLIENT_RECEIVER_PRIVATE_H

#include "network_data.h"
#include "client_state.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>


void connect_to_server(client_state* state);

void read_packet(client_state* state, uint32_t ptlen);

char* read_message(client_state* state, char* msg);

typedef struct {
    uint32_t nplen;
    uint32_t proto_version;
    uint16_t msg_number;
} network_packet_header;

typedef struct {
    uint16_t msglen;
    network_packet type;
} network_message_header;

#endif //DOOM_CLIENT_RECEIVER_PRIVATE_H
