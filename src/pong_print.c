#include "pong_print.h"



void print_waiting_client(field *f) {
	print_borders(f->left_offset, f->top_offset, f->width, f->height);
	attrset(COLOR_PAIR(1));
	mvprintw(3, 12, "PONG");
	mvprintw(4, 12, "Waiting for client to conect...");
}


void print_connecting_server(field *f) {
	print_borders(f->left_offset, f->top_offset, f->width, f->height);
	attrset(COLOR_PAIR(1));
	mvprintw(3, 20, "PONG");
	mvprintw(4, 20, "Connecting server...");
}


void print_selecting_mode(field *f, player *p1, player *p2) {
	print_borders(f->left_offset, f->top_offset, f->width, f->height);
	attrset(COLOR_PAIR(1));
	mvprintw(3, 20, "PONG");
	mvprintw(4, 20, "Select playing mode");

	mvprintw(7, 20, "5 goal game");
	mvprintw(8, 20, "7 goal game");
	mvprintw(9, 20, "9 goal game");

	attrset(COLOR_PAIR(4));
	if (p1->mode_confirmed) {
		mvprintw(p1->mode_selected + 7, 14, "OK ->");
	} else {
		mvprintw(p1->mode_selected + 7, 17, "->");
	}

	attrset(COLOR_PAIR(5));
	if (p2->mode_confirmed) {
		mvprintw(p2->mode_selected + 7, 32, "<- OK");
	} else {
		mvprintw(p2->mode_selected + 7, 32, "<-");
	}
}


void print_app(ball *b, field *f, player *p1, player *p2) {
	erase();
	mvprintw(0, 0 , f->debug);
	switch (f->game_status) {
		case STATUS_WAITING_CLIENT:
			print_waiting_client(f);
			break;
			
		case STATUS_CONNECTING_SERVER:
			print_connecting_server(f);
			break;

		case STATUS_SELECT_MODE:
			print_selecting_mode(f, p1, p2);
			break;

		case STATUS_GOAL:
		case STATUS_PLAYING:
		case STATUS_WAIT_START:
			print_game(b, f, p1, p2);
	}
	refresh();
}



void print_borders(int x_zero, int y_zero, int width, int height) {
	attrset(COLOR_PAIR(6));
	mvhline(y_zero, x_zero, ' ', width);
	mvhline(y_zero + height, x_zero, ' ', width);
	mvvline(y_zero, x_zero, ' ', height + 1);
	mvvline(y_zero, x_zero + width - 1, ' ', height + 1);
}


void print_player(int x, int top_y, int height) {
	mvvline(top_y, x, ' ', height);
}




void print_game (ball *b, field *f, player *p1, player *p2) {
	int center_line = SCENERY_WIDTH / 2;

	/* print field borders */
	print_borders(f->left_offset, f->top_offset, f->width, f->height);

	/* print field center line */
	attrset(COLOR_PAIR(2));
	for (int a = f->top_offset + 1; a < (f->height + f->top_offset); a++){
		mvprintw(a, center_line, " ");
	}
	
	/* print ball */
	/* select ball color */
	if (b->x_th == (center_line * 1000)) {
		attrset(COLOR_PAIR(2));
	} else {
		attrset(COLOR_PAIR(1));
	}
	mvprintw(f->top_offset + b->y_th / 1000, f->left_offset + b->x_th / 1000, "O");

	/* print player 1 */
	attrset(COLOR_PAIR(2));
	print_player(f->left_offset + p1->x, f->top_offset + p1->top_y, p1->slice_height);
	/* print player 2 */
	attrset(COLOR_PAIR(3));
	print_player(f->left_offset + p2->x, f->top_offset + p2->top_y, p2->slice_height);

	attrset(COLOR_PAIR(4));
	mvprintw(0, center_line - 5 - strlen(p1->name), p1->name);
	mvprintw(0, center_line - 4, "- %i", p1->score);

	attrset(COLOR_PAIR(5));
	mvprintw(0, center_line + 6, p2->name);
	mvprintw(0, center_line + 2, "%i -", p2->score);
}

