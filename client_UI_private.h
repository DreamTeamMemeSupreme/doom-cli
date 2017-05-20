//
// Created by snixosha on 29.04.17.
//

#ifndef DOOM_CLIENT_UI_PRIVATE_H
#define DOOM_CLIENT_UI_PRIVATE_H

void _ui_clear_status_bar();
void _ui_clear_team_list();
void _ui_clear_team_lobby();
void _ui_clear_screen();
char* _ui_get_key( bool* isEnd );
char* _ui_read_command( bool* isEnd );

#endif //DOOM_CLIENT_UI_PRIVATE_H
