//
// Created by snixosha on 27.04.17.
//

#ifndef DOOM_CLIENT_UI_H
#define DOOM_CLIENT_UI_H

#endif //DOOM_CLIENT_UI_H

#include <ncurses.h>

//read an input from a user
char* read_command();

//write msg in status bar
void write_msg(char* msg);

//display default UI
void ui_default();

//display list of teams in general lobby
void ui_team_list(char** teams);

//display lobby
//-players (players[0] is the creator of this lobby)
//-size (size of players)
void ui_team_lobby(char** players, uint16_t size);

//display team info
//-players (players[0] is the creator of this lobby)
//-size (size of players)
void ui_team_info(char* team_name, char** players, uint16_t size);

//display field
void ui_game_field(char** field);

//display state
void ui_game_state(uint16_t weapon_ban_timer, uint16_t health, uint16_t ammo_left);
