#include "pong.h"

int check_ball_goal_new(field *f, ball *b) {
	if (b->x_speed_th == 0) {
		return HIT_NO_HIT;
	}
	
	if (b->x_speed_th < 0) { //Moving UP
		if (b->x_th > 0) {
			return HIT_NO_HIT;
		} else {
			return HIT_LEFT_BORDER_HIT;
		}
	} else if (b->x_speed_th > 0) { //Moving DOWN
		if (b->x_th < (f->width * 1000)) {
			return HIT_NO_HIT;
		} else {
			return HIT_RIGHT_BORDER_HIT;
		}
	}
	
	return HIT_NO_HIT;
}



int check_ball_borders_hit_new(ball *b, field *f) {
	if (b->y_speed_th == 0) {
		return HIT_NO_HIT;
	}
	
	if (b->y_speed_th < 0) { //Moving UP
		if (b->y_th > 1000) {
			return HIT_NO_HIT;
		} else {
			return HIT_TOP_BORDER_HIT;
		}
	} else if (b->y_speed_th > 0) { //Moving DOWN
		if (b->y_th < (f->height * 1000 - 1000)) {
			return HIT_NO_HIT;
		} else {
			return HIT_BOTTOM_BORDER_HIT;
		}
	}
	
	return HIT_NO_HIT;
	
}


int check_ball_players_hit_new(ball *b, player *p1, player *p2, int *dist_from_center) {
	if (b->x_speed_th == 0) {
		return HIT_NO_HIT;
	} else if (b->x_speed_th < 0) {
		/* check for player1 hits */
		if (b->x_th / 1000 != (p1->x + 1)) {
			return HIT_NO_HIT;
		}

		if ((b->y_th / 1000) >= (p1->top_y) && (b->y_th / 1000) < ((p1->top_y) + p1->slice_height)) {
			int center_th = ((1000 * p1->top_y) + ((1000 * (p1->slice_height - 1)) / 2));
			*dist_from_center = (b->y_th) - center_th;
			return HIT_PLAYER1_HIT;
		}
	} else if (b->x_speed_th > 0) {
		/* check for player2 hits */
		if (b->x_th / 1000 != (p2->x - 1)) {
			return HIT_NO_HIT;
		}

		if ((b->y_th / 1000) >= p2->top_y && (b->y_th / 1000) < (p2->top_y + p2->slice_height)) {
			int center_th = ((1000 * p2->top_y) + ((1000 * (p2->slice_height - 1)) / 2));
			*dist_from_center = (b->y_th) - center_th;
			return HIT_PLAYER2_HIT;
		}

		return HIT_NO_HIT;
	}
	
	return HIT_NO_HIT;
}
