//
// Created by ivb on 21.05.17.
//

#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "client_general.h"
#include "client_general_private.h"
#include "client_receiver_private.h"

client_state state;

void init_app()
{
    set_handlers();
    config_state();
    ui_start();
    client_task_queue_init(&state.queue);
    launch_threads();
    freopen("stderr.log", "w", stderr);
}

void exec_app()
{
    fprintf(stderr, "Application started");
    ui_general();
    while(!state.quit) {
        client_task* task = client_task_queue_pop(&state.queue);
        if(task == NULL) {
            continue;
        }
        if(task->task_type == USER_INPUT) {
            process_user_input(task);
        }
        if(task->task_type == SERVER_RESPONSE) {
            process_server_response(task);
        }
        client_task_delete(task);
    }
}

void set_handlers()
{
    struct sigaction sa;

    sa.sa_flags = SA_NODEFER;

    // SIGINT: Interrupt from keyboard
    sa.sa_handler = sigint_handler;
    if(sigaction(SIGINT, &sa, NULL) < 0) {
        perror("ERROR: 'sigaction' in 'set_handlers' failed.");
        exit(errno);
    }
}

void config_state()
{
    state.stage = DEFAULT;
    state.input = COMMAND;
    state.connection = false;
    state.quit = false;
    state.page = 1;
    state.sockfd = 2;

    state.team_list = malloc(sizeof(response_team_list_data));
    state.team_list->count = 0;
    state.team_list->players = NULL;
    state.team_list->sizes = NULL;

    state.name[0] = '\0';

    state.team_info = malloc(sizeof(response_team_info_data));
    state.team_info->count = 0;
    state.team_info->size = 0;
}

void launch_threads()
{
    int pe;

    pe = pthread_create(&state.user_input, NULL,
                        &ui_read_input, (void*)&state);

    if(pe != 0) {
        perror("ERROR: 'pthread_create' failed");
        exit(errno);
    }

//    pe = pthread_create(&state.server_receive, NULL,
//                        client_receiver, (void*)&state);
//
//    if(pe != 0) {
//        perror("ERROR: 'pthread_create' failed");
//        exit(errno);
//    }
}

void sigint_handler(int s)
{
    normal_exit();
}


void process_server_response(client_task* task)
{
    switch(task->data_type.net_type) {
        case ERROR:
            process_server_error(task->data);
            break;

        case RESPONSE_FIELD_UPDATE:
            process_response_field_update(task->data);
            break;

        case RESPONSE_GAME_OVER:
            process_response_game_over(task->data);
            break;

        case RESPONSE_STATE_UPDATE:
            process_response_state_update(task->data);
            break;

        case RESPONSE_TEAM_INFO:
            process_response_team_info(task->data);
            break;

        case RESPONSE_TEAM_LIST:
            process_response_team_list(task->data);
            break;

        case RESPONSE_YOUR_TURN:
            process_response_your_turn(task->data);
            break;

        default:
            fprintf(stderr, "ERROR: unknown response from server\n");
    }
}

void process_server_error(void* data)
{
    game_error_data* error = (game_error_data*) data;

    ui_write_to_status_bar((char*)error->message);

    switch(error->err_code) {
        case UNKNOWN_COMMAND:
            break;

        case UNEXPECTED_COMMAND:
            break;

        case TEAM_CREATE_BAD_NAME:
            break;

        case TEAM_JOIN_BAD_NAME:
            break;

        case TEAM_NO_SUCH_TEAM:
            break;

        default:
            fprintf(stderr, "ERROR: Unknown error from server\n");
    }
}

void process_user_input(client_task* task)
{
    switch(task->data_type.input_type) {
        case COMMAND:
            process_command(task->data);
            break;

        case KEY_PRESSED:
            process_key_pressed(task->data);
            break;

        default:
            fprintf(stderr, "ERROR: unknown user input");
    }
}

void process_command(void* data)
{
    char command[81];

    strncpy(command, (char*)data, 81);
    switch(state.stage) {
        case DEFAULT:
            window_default(command);
            break;

        default:
            ui_write_to_status_bar("Commands are not available. Use keys instead");
            state.input = KEY_PRESSED;
            break;
    }
}

void process_key_pressed(void* data)
{
    char key = *(char*)data;
    key = tolower(key);

    switch(state.stage) {
        case INFO:
            window_info(key);
            break;

        case LIST:
            window_list(key);
            break;

        case GAME:
            window_game(key);
            break;

        default:
            ui_write_to_status_bar("Please, write a command");
            state.input = COMMAND;
            break;
    }
}

void process_response_field_update(void* data)
{

}

void process_response_game_over(void* data)
{

}

void process_response_state_update(void* data)
{

}

void process_response_team_info(void* data)
{

}

void process_response_team_list(void* data)
{

}

void process_response_your_turn(void* data)
{

}

void window_default(char command_line[81])
{
    char cmd[81];
    char arg[81];

    ui_write_to_status_bar("");
    parse_command(command_line, cmd, arg);
    if(cmd[0] == '\0') {
        ui_write_to_status_bar("Unexpected characters or empty command");
        return;
    }

    if(strcmp(cmd, "EXIT") == 0) {
        command_exit(arg);
    } else if(strcmp(cmd, "LIST") == 0) {
        command_list(arg);
    } else if(strcmp(cmd, "LOGIN") == 0) {
            command_login(arg);
    } else if(strcmp(cmd, "CREATE") == 0) {
            command_create(arg);
    } else if(strcmp(cmd, "JOIN") == 0) {
            command_join(arg);
    } else if(strcmp(cmd, "INFO") == 0) {
            command_info(arg);
    } else {
        ui_write_to_status_bar("Unknown command. Try again");
    }

}

void window_info(char key)
{
    ui_write_to_status_bar("");
    switch(key) {
        case 'z':
            --state.page;
            if(state.page < 1) state.page = 1;
            ui_update_team_info(state.team_info, state.page);
            break;

        case 'x':
            ++state.page;
            ui_update_team_info(state.team_info, state.page);
            break;

        case 'q':
            state.input = COMMAND;
            state.stage = DEFAULT;
            ui_general();
            break;

        default:
            ui_write_to_status_bar("Unknown key");
    }
}

void window_list(char key)
{
    ui_write_to_status_bar("");
    switch(key) {
        case 'z':
            --state.page;
            if(state.page < 1) state.page = 1;
            ui_update_team_list(state.team_list, state.page);
            break;

        case 'x':
            ++state.page;
            ui_update_team_list(state.team_list, state.page);
            break;

        case 'q':
            state.input = COMMAND;
            state.stage = DEFAULT;
            ui_general();
            break;

        default:
            ui_write_to_status_bar("Unknown key");
    }
}

void window_game(char key)
{

}

void normal_exit()
{
    state.quit = true;
    ui_close();
    pthread_join(state.user_input, NULL);
    pthread_join(state.server_receive, NULL);
    printf("Properly exited\n");
    exit(0);
}

void parse_command(char command_line[81], char cmd[81], char arg[81])
{
    int i;
    int cmd_b = 0, cmd_e = 0;
    int arg_b = 0, arg_e = 0;

    while(isspace(command_line[cmd_b])) ++cmd_b;
    if(!isalpha(command_line[cmd_b])) {
        cmd[0] = arg[0] = '\0';
        return;
    }
    cmd_e = cmd_b + 1;
    while(isalpha(command_line[cmd_e])) ++cmd_e;
    if(!isspace(command_line[cmd_e]) && command_line[cmd_e] != '\0') {
        cmd[0] = arg[0] = '\0';
        return;
    }
    strncpy(cmd, command_line + cmd_b, cmd_e - cmd_b);
    cmd[cmd_e - cmd_b] = '\0';
    arg_b = cmd_e;
    while(isspace(command_line[arg_b])) ++arg_b;
    if(command_line[arg_b] == '\0') {
        arg[0] = '\1';
        return;
    }
    if(!isalnum(command_line[arg_b])) {
        arg[0] = '\0';
        return;
    }
    arg_e = arg_b + 1;
    while(isalnum(command_line[arg_e])) ++arg_e;
    if(!isspace(command_line[arg_e]) && command_line[arg_e] != '\0') {
        arg[0] = '\0';
        return;
    }
    i = arg_e + 1;
    while(isspace(command_line[i])) ++i;
    if(command_line[i] != '\0') {
        arg[0] = '\0';
        return;
    }
    strncpy(arg, command_line + arg_b, arg_e - arg_b);
    arg[arg_e - arg_b] = '\0';
}

void command_login(char arg[81])
{
    char msg[81];

    if(arg[0] == '\0') {
        ui_write_to_status_bar("Unexpected characters in argument");
        return;
    }
    if(arg[0] == '\1') {
        if(state.name[0] == '\0') {
            ui_write_to_status_bar("You have no login yet");
            return;
        }
        sprintf(msg, "Your login: %s", state.name);
        ui_write_to_status_bar(msg);
        return;
    }
    if(state.name[0] != '\0') {
        ui_write_to_status_bar("To change login, please, reopen application");
        return;
    }
    strcpy(state.name, arg);
    sprintf(msg, "Hello, %s!", state.name);
    ui_write_to_status_bar(msg);
}

void command_join(char arg[81])
{
    char msg[81];
    if(state.name[0] == '\0') {
        ui_write_to_status_bar("To join, please, enter your login");
        return;
    }
    if(arg[0] == '\0') {
        ui_write_to_status_bar("Unexpected characters in argument");
        return;
    }
    if(arg[0] == '\1') {
        ui_write_to_status_bar("Expected team name");
        return;
    }
    sprintf(msg, "Joining to '%s'...", arg);
    ui_write_to_status_bar(msg);
    request_team_join(arg);
}

void command_create(char arg[81])
{
    char msg[81];
    if(state.name[0] == '\0') {
        ui_write_to_status_bar("To create new team, please, enter your login");
        return;
    }
    if(arg[0] == '\0') {
        ui_write_to_status_bar("Unexpected characters in argument");
        return;
    }
    if(arg[0] == '\1') {
        ui_write_to_status_bar("Expected team name");
        return;
    }
    sprintf(msg, "Creating team '%s'...", arg);
    ui_write_to_status_bar(msg);
    request_team_create(arg, 3);
}

void command_list(char arg[81])
{
    if(arg[0] != '\1') {
        ui_write_to_status_bar("Unexpected argument");
        return;
    }
    state.stage = LIST;
    state.input = KEY_PRESSED;
    state.page = 1;
    ui_team_list(state.team_list);
    ui_write_to_status_bar("Updating team list...");
    request_team_list();
}

void command_exit(char arg[81])
{
    if(arg[0] != '\1') {
        ui_write_to_status_bar("Unexpected argument");
        return;
    }
    normal_exit();
}

void command_info(char arg[81])
{
    char msg[81];
    if(arg[0] == '\0') {
        ui_write_to_status_bar("Unexpected characters or empty argument");
        return;
    }
    if(arg[0] == '\1') {
        ui_write_to_status_bar("Expected team name");
        return;
    }
    state.stage = INFO;
    state.input = KEY_PRESSED;
    state.page = 1;
    state.team_info->count = 0;
    sprintf(msg, "Getting information about team '%s'...", arg);
    ui_team_info(state.team_info);
    ui_write_to_status_bar(msg);
    request_team_info(arg);
}

void request_disconnect()
{
    request_empty(DISCONNECT);
}

void request_team_create(char team_name[81], uint16_t size)
{
    void* messages[2];
    team_create_data tcdata;
    team_join_data tjdata;

    tcdata.size = size;
    strncpy(tcdata.team_name, team_name, 60);

    strncpy(tjdata.player_name, state.name, 60);
    strncpy(tjdata.team_name, team_name, 60);

    create_message(TEAM_CREATE, (void*)&tcdata, sizeof(team_create_data), &(messages[0]));
    create_message(TEAM_JOIN, (void*)&tjdata, sizeof(team_join_data), &(messages[1]));

    send_messages(messages, 2);
}

void request_team_join(char team_name[81])
{
    void* messages[1];
    team_join_data tjdata;

    strncpy(tjdata.player_name, state.name, 60);
    strncpy(tjdata.team_name, team_name, 60);

    create_message(TEAM_JOIN, (void*)&tjdata, sizeof(team_join_data), &(messages[0]));

    send_messages(messages, 1);
}

void request_team_leave()
{
    request_empty(TEAM_LEAVE);
}

void request_team_list()
{
    request_empty(TEAM_LIST);
}

void request_team_info(char team_name[81])
{
    void* messages[1];
    team_info_data tidata;

    strncpy(tidata.team_name, team_name, 60);

    send_messages(messages, 1);
}

void request_game_start()
{
    request_empty(GAME_START);
}

void request_game_use_item()
{
    request_empty(GAME_USE_ITEM);
}

void request_game_use_charge()
{
    request_empty(GAME_USE_CHARGE);
}

void request_game_move(game_direction direction)
{
    void* messages[1];
    game_move_data gmdata;

    gmdata.direction = direction;
    create_message(GAME_MOVE, (void*)&gmdata, sizeof(game_move_data), &(messages[1]));
    send_messages(messages, 1);
}

void request_game_skip_turn()
{
    request_empty(GAME_SKIP_TURN);
}

void request_empty(network_packet type)
{
    void* messages[1];
    create_message(type, NULL, 0, &(messages[0]));
    send_messages(messages, 1);
}

uint32_t create_message(network_packet type, void* data, size_t ds, void** dest)
{
    network_message_header header;

    header.msglen = sizeof(network_message_header) + ds;
    header.type = type;

    *dest = malloc(header.msglen);

    *(network_message_header*)(*dest) = header;

    strncpy((char*)((*dest) + sizeof(network_message_header)), (char*)data, ds);

    return header.msglen;
}

uint32_t create_packet(void** messages, uint32_t mn, void** dest)
{
    int i;
    void* cur;
    network_packet_header header;

    header.msg_number = mn;
    header.proto_version = PROTO_VERSION;

    header.nplen = sizeof(network_packet_header);

    for(i = 0; i < mn; ++i) {
        header.nplen += ((network_message_header*)messages[i])->msglen;
    }

    *dest = malloc(header.nplen);
    *(network_packet_header*)(*dest) = header;
    cur = (*dest) + sizeof(network_packet_header);
    for(i = 0; i < mn; ++i) {
        strncpy((char*)cur, (char*)messages[i], ((network_message_header*)messages[i])->msglen);
        cur += ((network_message_header*)messages[i])->msglen;
        free(messages[i]);
    }

    return header.nplen;
}

void send_messages(void** messages, uint32_t mn)
{
    void* packet;
    uint32_t nplen;

    nplen = create_packet(messages, mn, &packet);

    if(write(state.sockfd, packet, nplen) < 0) {
        perror("ERROR: write to socket failed");
    }

    free(packet);
}