//
// Created by ivb on 21.05.17.
//

#ifndef DOOM_CLIENT_GENERAL_H
#define DOOM_CLIENT_GENERAL_H

#include "client_state.h"

// launches user interface and server listener
// configures data structures for work
void init_app();

// starts main loop of doing tasks from queue
void exec_app();

#endif //DOOM_CLIENT_GENERAL_H
