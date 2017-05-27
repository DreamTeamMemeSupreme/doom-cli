//
// Created by snixosha on 27.04.17.
//
#include "client_UI.h"
#include "client_UI_private.h"
#include "client_state.h"

void ui_start() {
    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    move(31,2);
    pthread_mutex_init(&block_input,NULL);
}

void ui_close() {
    endwin();
    pthread_mutex_destroy(&block_input);
}

void* ui_read_input( void* data ) {
    client_state* state = (client_state*) data;
    while( state->quit == 0 ) {
        if( state->input == COMMAND ) {
            char* command = _ui_read_command(&(state->quit), &state->input, state->input );
            if( command == 0 ) {
                if( state->input != COMMAND)
                    continue;
                break;
            }
            client_task* task = malloc(sizeof(client_task));
            task->task_type = USER_INPUT;
            task->data_type.input_type = COMMAND;
            task->data = (void*)command;
            client_task_queue_push(&state->queue, task);
        } else {
            char* key = _ui_get_key(&(state->quit), &state->input, state->input );
            if( key == 0 ) {
                if (state->input != KEY_PRESSED)
                    continue;
                break;
            }
            client_task* task = malloc(sizeof(client_task));
            task->task_type = USER_INPUT;
            task->data_type.input_type = KEY_PRESSED;
            task->data = (void*)key;
            client_task_queue_push(&state->queue, task);
        }
    }
	pthread_exit(0);
}

void ui_write_to_status_bar( char* msg ) {
    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    _ui_clear_status_bar();
    mvprintw(29,1," %s",msg);

    move(t_y, t_x);
    pthread_mutex_unlock(&block_input);
}

void ui_general() {

    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    _ui_clear_screen();
    mvprintw(0,0,"===============================================================");
    mvprintw(1,0,"= DooM                                                        =");
    mvprintw(2,0,"===============================================================");
    for( int i = 3; i < 33; i++ ) {
        mvprintw(i,0,"=                                                             =");
    }
    mvprintw(4,1," Commands in main menu:");
    mvprintw(6,1," LOGIN <name>          |  login in a client");
    mvprintw(7,1," CREATE <team-name>    |  create a new team");
    mvprintw(8,1," JOIN <team-name>      |  join in a team you want");
    mvprintw(9,1," INFO <team-name>      |  get information about team");
    mvprintw(10,1," LIST                  |  list existing teams");
    mvprintw(11,1," EXIT                  |  exit");

    mvprintw(28,0,"===============================================================");
    mvprintw(30,0,"===============================================================");
    mvprintw(32,0,"===============================================================");

    move(t_y,t_x);
    pthread_mutex_unlock(&block_input);

}

void ui_team_list(response_team_list_data* team_list) {
    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    _ui_clear_screen();
    mvprintw(1,1,"List of teams");
    mvprintw(4,2,"Number of teams: %d",team_list->count);

    move(t_y,t_x);
    pthread_mutex_unlock(&block_input);

    ui_update_team_list(team_list,1);
}

void ui_update_team_list(response_team_list_data* team_list, int page) {

    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    _ui_clear_team_list();

    int max_page = MAX(1, ( team_list->count / NUMBER_OF_TEAMS_ON_PAGE ) + (team_list->count % NUMBER_OF_TEAMS_ON_PAGE != 0));
    page = ( (page-1)%max_page + max_page)%max_page + 1;
    char buff[54];

    int s_i = (page-1)*NUMBER_OF_TEAMS_ON_PAGE;
    for( int i = s_i; i < MIN(s_i+NUMBER_OF_TEAMS_ON_PAGE,team_list->count); i++ ) {
        size_t t_n = MIN(53,strlen(team_list->teams[i]));
        strncpy(buff,team_list->teams[i],t_n);
        buff[t_n] = '\0';
        mvprintw(6+i-s_i,2,"%s",buff);
        mvprintw(6+i-s_i,56,"%d",team_list->players[i]);
        mvprintw(6+i-s_i,58,"/");
        mvprintw(6+i-s_i,59,"%d",team_list->sizes[i]);
    }

    mvprintw(26,2,"Page: %d/%d (press z/x to move between pages, q to quit)",page,max_page);

    move(t_y,t_x);
    pthread_mutex_unlock(&block_input);

}

void ui_team_lobby(response_team_info_data* team_info) {
    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    _ui_clear_screen();
    mvprintw(1,1,"Team lobby");
    mvprintw(4,2,"Number of players: %d/%d",team_info->count, team_info->size);

    move(t_y,t_x);
    pthread_mutex_unlock(&block_input);

    ui_update_team_lobby(team_info,1);
}

void ui_update_team_lobby(response_team_info_data* team_info, int page) {

    ui_update_team_info(team_info, page);
}

void ui_team_info(response_team_info_data* team_info) {
    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    _ui_clear_screen();
    mvprintw(1,1,"Team information");
    mvprintw(4,2,"Number of players: %d/%d",team_info->count, team_info->size);

    move(t_y,t_x);
    pthread_mutex_unlock(&block_input);

    ui_update_team_info(team_info,1);
}

void ui_update_team_info(response_team_info_data* team_info, int page) {

    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    _ui_clear_team_info();

    int max_page = MAX(1,(team_info->count / NUMBER_OF_TEAMS_ON_PAGE ) + (team_info->count % NUMBER_OF_TEAMS_ON_PAGE != 0));
    page = ( (page-1)%max_page + max_page)%max_page + 1;
    char buff[54];

    int s_i = (page-1)*NUMBER_OF_TEAMS_ON_PAGE;
    for( int i = s_i; i < MIN(s_i+NUMBER_OF_TEAMS_ON_PAGE,team_info->count); i++ ) {
        size_t t_n = MIN(53,strlen(team_info->players[i]));
        strncpy(buff,team_info->players[i],t_n);
        buff[t_n] = '\0';
        mvprintw(6+i-s_i,2,"%s",buff);
        if(s_i == 0)
            mvprintw(6,1,"*");
    }

    mvprintw(26,2,"Page: %d/%d (press z/x to move between pages, q to quit)",page,max_page);

    move(t_y,t_x);
    pthread_mutex_unlock(&block_input);
}

void ui_game(response_game_started* game_started) {

    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    _ui_clear_screen();
    mvprintw(1,1,"Game");
    mvprintw(12,44,"Health");
    mvprintw(14,44,"Ammo");
    mvprintw(16,44,"Ban timer");
    mvprintw(11,2,"WASD - move");
    mvprintw(13,2,"F - attack");
    mvprintw(15,2,"E - use items");
    mvprintw(17,2,"Q - quit");
    mvprintw(4,20,"=======================");
    mvprintw(26,20,"=======================");
    for( int i = 5; i < 26; i++ )
        mvprintw(i,20,"=                     =");

    move(t_y,t_x);
    pthread_mutex_unlock(&block_input);

    ui_update_game_field(&game_started->field);
    ui_update_game_state(&game_started->state);

}

void ui_update_game_field(response_field_update_data* field) {

    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    for( int i = 0; i < 21; i++ ) {
        move(5+i,21);
        for( int j = 0; j < 21; j++ )
            printw("%c",field->field[i][j]);
    }

    move(t_y,t_x);
    pthread_mutex_unlock(&block_input);
}

void ui_update_game_state(response_state_update_data* state) {
    pthread_mutex_lock(&block_input);
    int t_y, t_x;
    getyx(stdscr,t_y,t_x);

    _ui_clear_game_state();
    mvprintw(12,54,"%d",state->health);
    mvprintw(14,54,"%d",state->ammo_left);
    mvprintw(16,54,"%d",state->weapon_ban_timer);

    move(t_y,t_x);
    pthread_mutex_unlock(&block_input);
}

char* _ui_get_key( atomic_int* isEnd, input_type* type, input_type expected ) {
    char* c = malloc(sizeof(char));
    for(;;) {
        if( *isEnd == 1 || *type != expected ) {
            free(c);
            c = 0;
            break;
        }
        pthread_mutex_lock(&block_input);
        *c = getch();
        pthread_mutex_unlock(&block_input);
        if( *c > 0 ) {
            break;
        }
        //SLEEP(10);
    }
    return c;
}

char* _ui_read_command( atomic_int* isEnd, input_type* type, input_type expected  ) {
    char* input = malloc(sizeof(char)*MAX_INPUT_SIZE);
    int i = 0;
    while( true ) {
        if( *isEnd == 1 ) {
            free(input);
            input = 0;
            break;
        }
        char* new_key = _ui_get_key( isEnd, type, expected );
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
            if(i < 0) {
                i = 0;
                continue;
            }
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
    _ui_clear_input_line();
    return input;
}

void _ui_clear_status_bar() {
    mvprintw(29,1,"                                                             ");
}

//threadsafe
void _ui_clear_input_line() {
    pthread_mutex_lock(&block_input);
    mvprintw(31,1,"                                                             ");
    move(31,1);
    pthread_mutex_unlock(&block_input);
}

void _ui_clear_team_list() {
    int i = 6;
    for(; i < 27; i++ )
        mvprintw(i,1,"                                                             ");
}

void _ui_clear_team_info() {
    int i = 6;
    for(; i < 27; i++ )
        mvprintw(i,1,"                                                             ");
}

void _ui_clear_game_state() {
    mvprintw(12,54,"        ");
    mvprintw(14,54,"        ");
    mvprintw(16,54,"        ");
}

void _ui_clear_screen() {
    mvprintw(1,1,"                                                             ");
    mvprintw(29,1,"                                                             ");
    int i = 3;
    for(; i < 28; i++ )
        mvprintw(i,1,"                                                             ");
}
