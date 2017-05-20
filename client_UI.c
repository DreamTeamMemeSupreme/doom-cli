//
// Created by snixosha on 27.04.17.
//
#include "client_UI.h"
#include "client_UI_private.h"
#include "network_data.h"
#include "client_task.h"
#include "client_state.h"
#include <string.h>
#include <stdlib.h>
#define MAX_INPUT_SIZE 100
#define MAX_DISPLAYED_NAME_LENGTH 60
#define NUMBER_OF_TEAMS_ON_PAGE 20
#define NUMBER_OF_PLAYERS_ON_PAGE 2
#define STATUS_BAR_LINE 27
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))


void ui_start() {
    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    mvprintw(0,0,"============================DOOM============================");
    mvprintw(2,0,"============================================================");
    mvprintw(26,0,"============================================================");
    mvprintw(28,0,"============================================================");
    mvprintw(30,0,"============================================================");
}

void ui_close() {
    endwin();
}

void* ui_read_input( void* data ) {
    client_state* state = (client_state*) data;
    while( !state->quit ) {
        if( state->input == COMMAND ) {
            char* command = _ui_read_command(&(state->quit));
            if( command == 0 ) {
                break;
            }
            client_task* task = malloc(sizeof(client_task));
            task->task_type = USER_INPUT;
            task->data_type.input_type = COMMAND;
            task->data = (void*)command;
            client_task_queue_push(&state->queue, task);
        } else {
            char* key = _ui_get_key(&(state->quit));
            if( key == 0 )
                break;
            client_task* task = malloc(sizeof(client_task));
            task->task_type = USER_INPUT;
            task->data_type.input_type = KEY_PRESSED;
            task->data = (void*)key;
            client_task_queue_push(&state->queue, task);
        }
    }
}

char* _ui_get_key( bool* isEnd ) {
    char* c = malloc(sizeof(char));
    for(;;) {
        if( *isEnd == true) {
            free(c);
            c = 0;
            break;
        }
        if( (*c = getch()) > 0 ) {
            break;
        }
    }
    return c;
}

char* _ui_read_command( bool* isEnd ) {
    char* input = malloc(sizeof(char)*MAX_INPUT_SIZE);
    int i = 0;
    while( true ) {
        if( *isEnd == true ) {
            free(input);
            input = 0;
            break;
        }
        char* new_key = _ui_get_key( isEnd );
        if( new_key == 0) {
            free(input);
            input = 0;
            break;
        }
        input[i] = *(new_key);
        free(new_key);
        if(input[i] == 10) {
            input[i] = 0;
            break;
        }
        if(input[i] == 127) {
            i--;
            int t_x, t_y;
            getyx(stdscr,t_y,t_x);
            mvprintw(t_y,t_x-1," ");
            move(t_y,t_x-1);
            continue;
        }
        if( i < MAX_INPUT_SIZE - 1)
            printw("%c",input[i]);
        i = MIN(i+1,MAX_INPUT_SIZE - 1);
    }
    return input;
}

void _ui_clear_status_bar() {
    mvprintw(STATUS_BAR_LINE,0,"                                                                                ");
}

void _ui_clear_team_list() {
    int i = 0;
    for(; i < NUMBER_OF_TEAMS_ON_PAGE; i++ )
        mvprintw(i+3,0,"                                                                      ");
}

void _ui_clear_team_lobby() {
    int i = 0;
    for(; i < NUMBER_OF_PLAYERS_ON_PAGE; i++ )
        mvprintw(i+6,0,"                                                                      ");
}

void _ui_clear_screen() {
    mvprintw(1,0,"                                                                      ");
    int i = 3;
    for(; i < STATUS_BAR_LINE-1; i++ )
        mvprintw(i,0,"                                                                      ");
}
