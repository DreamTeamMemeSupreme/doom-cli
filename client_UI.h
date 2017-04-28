//
// Created by snixosha on 27.04.17.
//

#ifndef DOOM_CLIENT_UI_H
#define DOOM_CLIENT_UI_H

#include <ncurses.h>
#include "network_data.h"

//get a key
char ui_get_key();

//read an input from a user
char* ui_read_input();

//write msg in status bar
void ui_write_msg(char* msg);

//display default UI
void ui_start();

//turn off ncurses
void ui_close();

//display list of teams in general lobby
void ui_team_list(response_team_list_data* team_list);

//display lobby
void ui_team_lobby(response_team_info_data* team_info);

//display team info
void ui_team_info(response_team_info_data* team_info);

//display field
void ui_game_field(response_field_update_data* field);

//display state
void ui_game_state(response_state_update_data* state);

#endif //DOOM_CLIENT_UI_H