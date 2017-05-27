//
// Created by snixosha on 27.04.17.
//

#ifndef DOOM_CLIENT_UI_H
#define DOOM_CLIENT_UI_H

#include <ncurses.h>
#include "network_data.h"
#include "client_task_queue.h"
#include "client_state.h"
#include "client_task.h"
#include <string.h>
#include <stdlib.h>
#include <stdatomic.h>
#define MAX_INPUT_SIZE 60
#define NUMBER_OF_TEAMS_ON_PAGE 18

//display default UI
void ui_start();

//turn off ncurses
void ui_close();

//to be run in thread, data=client_state, saves all user input in queue
void* ui_read_input( void* data );

//write message to status bar
void ui_write_to_status_bar( char* msg );

//initial display of general lobby
void ui_general();

//initial display of list of teams in general lobby
void ui_team_list(response_team_list_data* team_list);

//shows needed page of team_list in list stage
void ui_update_team_list(response_team_list_data* team_list, int page);

//initial display of lobby
void ui_team_lobby(response_team_info_data* team_info);

//shows needed page of team_info in pre-game lobby
void ui_update_team_lobby(response_team_info_data* team_info, int page);

//initial display of team info
void ui_team_info(response_team_info_data* team_info);

//shows needed page of team_info in info stage
void ui_update_team_info(response_team_info_data* team_info, int page);

//initial display of game in game stage
void ui_game(response_game_started* game_started);

//updates field in game stage
void ui_update_game_field(response_field_update_data* field);

//updates game_state in game stage
void ui_update_game_state(response_state_update_data* state);

#endif //DOOM_CLIENT_UI_H
