#ifndef PONG_H
#define PONG_H

#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <poll.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
 

typedef struct {
	int16_t top_y;
	int16_t x;
	int8_t slice_height;
	int8_t score;
	char name[16];
	int8_t mode_selected;
	uint8_t mode_confirmed;
} player;

typedef struct {
	int16_t x_th;
	int16_t y_th;
	int16_t x_speed_th;
	int16_t y_speed_th;
} ball;

typedef struct {
	int8_t width;
	int8_t height;
	int8_t left_offset;
	int8_t top_offset;
	int8_t game_status;
	char debug[128];
} field;

enum {
	COLOR_DEFAULT = 1,
	COLOR_PLAYER_1 = 2,
	COLOR_PLAYER_2 = 3
};

enum {
	HIT_NO_HIT = 0,
	HIT_TOP_BORDER_HIT = 1,
	HIT_BOTTOM_BORDER_HIT = 2,
	HIT_LEFT_BORDER_HIT = 4,
	HIT_RIGHT_BORDER_HIT = 8,
	HIT_PLAYER1_HIT = 16,
	HIT_PLAYER2_HIT = 32
};

enum {
	BALL_DIR_STOP = 0,
	BALL_DIR_LEFT,
	BALL_DIR_RIGHT
};

enum {
	STATUS_WAITING_CLIENT,
	STATUS_CONNECTING_SERVER,
	STATUS_SELECT_MODE,
	STATUS_PLAYING,
	STATUS_GOAL,
	STATUS_WAIT_START
};

enum {
	MODE_5_GOALS,
	MODE_7_GOALS,
	MODE_9_GOALS,
	MODE_NUM_MODES
};

enum {
	MESSAGE_WELCOME_TO_SRV,
	MESSAGE_WELCOME_TO_CLI,
	MESSAGE_PLAYER_POS,
	MESSAGE_BALL_POS,
	MESSAGE_MENU_SEL,
	MESSAGE_MENU_SEL_CNFRM,
	MESSAGE_MENU_SEL_NCNFRM,
	MESSAGE_GAME_STATUS_CHANGE
};


#define BUFLEN 					512	   //Max length of buffer
#define PORT					8001   //The port on which to listen (or connect) for incoming data1

#define SCENERY_WIDTH			71
#define SCENERY_TOP_OFFSET 		1
#define SCENERY_LEFT_OFFSET		0
#define SCENERY_HEIGHT			30
#define PONG_HEIGHT				5

#define SLICE_HEIGHT			6
#define SLICE_GOAL_SEPARATION	2

#define PLAYER_NAME_MAX_LEN		16

void initialize_screen();
void set_player (player *p, int x, int top_y, int slice_height, int score, char *name);
void set_field (field *f, int left_offset, int top_offset, int width, int height);
void print_borders(int x_zero, int y_zero, int width, int height);
void print_player(int x, int top_y, int height);
void print_game (ball *b, field *f, player *p1, player *p2);
void print_waiting_client(field *f);
void print_selecting_mode(field *f, player *p1, player *p2) ;
void move_player (int16_t *player_top_y, int slice_height, int top_border, int bottom_border, int position_increment);
void print_app(ball *b, field *f, player *p1, player *p2);
int check_ball_goal_new(field *f, ball *b) ;
int check_ball_borders_hit_new(ball *b, field *f);
int check_ball_players_hit_new(ball *b, player *p1, player *p2, int *dist_from_center);
int main(int argc, char **argv);

void send_network_message(int type, ball *b, field *f, player *p1, player *p2);

#endif //PONG_H