//
//  network_data.h
//  doom-cli
//
//  Created by Anton Suslov on 15/04/2017.
//  Copyright © 2017 Anton Suslov. All rights reserved.
//

#ifndef network_data_h
#define network_data_h

#include <stdint.h>

typedef enum {
	ERROR = 0,
	DISCONNECT,
	TEAM_CREATE,
	TEAM_JOIN,
	TEAM_LEAVE,
	TEAM_LIST,
	TEAM_INFO,
	GAME_START,
	GAME_USE_ITEM,
	GAME_USE_CHARGE,
	GAME_MOVE,
	GAME_SKIP_TURN,
	RESPONSE_FIELD_UPDATE,
	RESPONSE_STATE_UPDATE,
	RESPONSE_YOUR_TURN,
	RESPONSE_GAME_OVER
} network_packet;

typedef enum {
	UKNOWN_COMMAND = 0,
	UNEXPECTED_COMMAND,
	TEAM_JOIN_BAD_NAME,
	TEAM_CREATE_BAD_NAME,
	TEAM_NO_SUCH_TEAM,
} error_type;

typedef enum {
	DIR_UP = 0,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT
} game_direction;

typedef struct {
	uint16_t err_code;
	void *message;
} game_error_data;

typedef struct {
	uint16_t size;
	char *team_name;
} team_create_data;

typedef struct {
	char *team_name;
	char *player_name;
} team_join_data;

typedef struct {
	game_direction direction;
} game_move_data;

typedef struct {
	char field[21][21];
} response_field_update_data;

typedef struct {
	uint16_t weapon_ban_timer;
	uint16_t health;
	uint16_t ammo_left;
} response_state_update_data;

#endif /* network_data_h */
