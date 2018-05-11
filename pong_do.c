#include "pong_do.h"

void do_game(ball *b, field *f, player *p1, player *p2) {
	int hit_check = 0;
	int dist_from_center = 0;
	//Move ball
	b->y_th += b->y_speed_th;
	b->x_th += b->x_speed_th;

	//players hits rock
	hit_check = check_ball_players_hit_new(b, p1, p2, &dist_from_center);
	switch (hit_check) {
		case HIT_PLAYER1_HIT:
			b->x_speed_th *= -1;
			b->y_speed_th = dist_from_center / 4;
			break;

		case HIT_PLAYER2_HIT:
			b->x_speed_th *= -1;
			b->y_speed_th = dist_from_center / 4;
			break;
	}


	//check ball hits wall
	hit_check = check_ball_borders_hit_new(b, f);

	switch (hit_check) {
		case HIT_TOP_BORDER_HIT:
			b->y_speed_th *= -1;
			//ball_y_direction_th *= -1;
			break;

		case HIT_BOTTOM_BORDER_HIT:
			b->y_speed_th *= -1;
			//ball_y_direction_th *= -1;
			break;
	}

	//check ball scores goal
	hit_check = check_ball_goal_new(f, b);

	switch (hit_check) {
		case HIT_LEFT_BORDER_HIT:
			b->x_th = 24000;
			b->y_th = 5000;
			b->y_speed_th = 0;
			p2->score++;
			break;

		case HIT_RIGHT_BORDER_HIT:
			b->x_th = 24000;
			b->y_th = 5000;
			b->y_speed_th = 0;
			p1->score++;
			break;
	}
	
}




void do_keyboard(int key, ball *b, field *f, player *p1, player *p2) {
	switch (f->game_status) {
		case STATUS_CONNECTING_SERVER:
		case STATUS_WAITING_CLIENT:
			// do nothing
			break;

		case STATUS_SELECT_MODE:
			switch (key) {
				case 10: //ENTER KEY;
					p1->mode_confirmed = 1;

					send_network_message(MESSAGE_MENU_SEL_CNFRM, b, f, p1, p2);

					if ((p1->mode_confirmed && p2->mode_confirmed) && (p1->mode_selected == p2->mode_selected))
						f->game_status = STATUS_PLAYING;
					break;

				case KEY_DOWN:
					if (p1->mode_confirmed) break;
					p1->mode_selected++;
					if (p1->mode_selected >= MODE_NUM_MODES) p1->mode_selected = 0;
					send_network_message(MESSAGE_MENU_SEL, b, f, p1, p2);
					break;

				case KEY_UP:
					if (p1->mode_confirmed) break;
					p1->mode_selected--;
					if (p1->mode_selected < 0) p1->mode_selected = MODE_NUM_MODES - 1;
					send_network_message(MESSAGE_MENU_SEL, b, f, p1, p2);
					break;

				case KEY_BACKSPACE:
					p1->mode_confirmed = 0;
					send_network_message(MESSAGE_MENU_SEL_NCNFRM, b, f, p1, p2);
					break;

			}
			break;


		case STATUS_GOAL:
		case STATUS_PLAYING:

			switch (key) {
				case KEY_UP:
					move_player(&p1->top_y, SLICE_HEIGHT, 0, SCENERY_HEIGHT, -1);
					send_network_message(MESSAGE_PLAYER_POS, b, f, p1, p2);
					break;
				case KEY_DOWN:
					move_player(&p1->top_y, SLICE_HEIGHT, 0, SCENERY_HEIGHT, 1);
					send_network_message(MESSAGE_PLAYER_POS, b, f, p1, p2);
					break;
				default:
					move_player(&p1->top_y, SLICE_HEIGHT, 0, SCENERY_HEIGHT, 0);
					send_network_message(MESSAGE_PLAYER_POS, b, f, p1, p2);
			}
			break;

		case STATUS_WAIT_START:
			//do not do nothing while waiting for game to start
			break;
	}
}



void do_network(ball *b, field *f, player *p1, player *p2, uint8_t *buffer, int buffer_size)
{
	
	uint16_t x = 0;
	uint16_t y = 0;

	switch (buffer[0]) {
		case MESSAGE_WELCOME_TO_SRV:
			strncpy(p2->name, (char *)buffer + 1, PLAYER_NAME_MAX_LEN);
			send_network_message(MESSAGE_WELCOME_TO_CLI, b, f, p1, p2);
			f->game_status = STATUS_SELECT_MODE;
			break;

		case MESSAGE_WELCOME_TO_CLI:
			f->width = buffer[1];
			f->height = buffer[2];
			strncpy(p2->name, (char *)buffer + 3, PLAYER_NAME_MAX_LEN);
			f->game_status = STATUS_SELECT_MODE;
			break;

		case MESSAGE_MENU_SEL:
			p2->mode_selected = buffer[1];
			break;

		case MESSAGE_MENU_SEL_CNFRM:
			p2->mode_confirmed = 1;

			if ((p1->mode_confirmed && p2->mode_confirmed) && (p1->mode_selected == p2->mode_selected))
				f->game_status = STATUS_PLAYING;
			break;

		case MESSAGE_MENU_SEL_NCNFRM:
			p2->mode_confirmed = 0;
			break;

		case MESSAGE_BALL_POS:
			strcpy(f->debug, "B: ");
			f->debug[3] = 0x30 + buffer[1];
			f->debug[4] = 0x30 + buffer[2];
			f->debug[5] = 0x30 + buffer[3];
			f->debug[6] = 0x30 + buffer[4];

			x = (buffer[1] << 8) | buffer[2];
			y = (buffer[3] << 8) | buffer[4];
			b->x_th = x;
			b->y_th = y;
			break;

		case MESSAGE_PLAYER_POS:
			strcpy(f->debug, "P: ");
			f->debug[3] = 0x30 + buffer[1];
			f->debug[4] = 0x30 + buffer[2];
			f->debug[5] = 0x30 + buffer[3];
			f->debug[6] = 0x30 + buffer[4];
			f->debug[7] = ' ';
			f->debug[8] = y;

			x = (buffer[1] << 8) | buffer[2];
			y = (buffer[3] << 8) | buffer[4];
			//p2->x = x;
			p2->top_y = y;
			break;

	}
		
}
