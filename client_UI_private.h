//
// Created by snixosha on 29.04.17.
//

#ifndef DOOM_CLIENT_UI_PRIVATE_H
#define DOOM_CLIENT_UI_PRIVATE_H

#include <pthread.h>
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )

pthread_mutex_t block_input;

void _ui_clear_status_bar();
void _ui_clear_input_line();

void _ui_clear_team_list();
void _ui_clear_team_lobby();
void _ui_clear_game_state();
void _ui_clear_team_info();
void _ui_clear_screen();

char* _ui_get_key( atomic_int* isEnd );
char* _ui_read_command( atomic_int* isEnd );

#endif //DOOM_CLIENT_UI_PRIVATE_H
