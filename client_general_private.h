//
// Created by ivb on 21.05.17.
//

#ifndef DOOM_CLIENT_GENERAL_PRIVATE_H
#define DOOM_CLIENT_GENERAL_PRIVATE_H

#include "client_receiver.h"
#include "client_state.h"
#include "client_UI.h"
#include "client_task_queue.h"

#include "client_receiver_private.h"

void set_handlers();

void sigint_handler();

void config_state();

void launch_threads();

void process_user_input(client_task* task);

void process_server_response(client_task* task);

void process_server_error(void* data);

void process_response_field_update(void* data);

void process_response_game_over(void* data);

void process_response_state_update(void* data);

void process_response_team_info(void* data);

void process_response_team_list(void* data);

void process_response_your_turn(void* data);

void process_command(void* data);

void process_key_pressed(void* data);

void window_default(char command[81]);

void window_info(char key);

void window_list(char key);

void window_game(char key);

void process_key(char key);

void normal_exit();

void request_disconnect();

void request_team_create(char team_name[81], uint16_t size);

void request_team_join(char team_name[81]);

void request_team_leave();

void request_team_list();

void request_team_info(char team_name[81]);

void request_game_start();

void request_game_use_item();

void request_game_use_charge();

void request_game_move(game_direction direction);

void request_game_skip_turn();

void request_empty(network_packet type);

void send_messages(void** messages, uint32_t mn);

void parse_command(char command_line[81], char cmd[81], char arg[81]);

void command_login(char arg[81]);

void command_join(char arg[81]);

void command_create(char arg[81]);

void command_list(char arg[81]);

void command_exit(char arg[81]);

void command_info(char arg[81]);


uint32_t create_message(network_packet type, void* data, size_t ds, void** dest);
uint32_t create_packet(void** messages, uint32_t mn, void** dest);

#endif //DOOM_CLIENT_GENERAL_PRIVATE_H
