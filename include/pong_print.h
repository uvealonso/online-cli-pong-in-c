#ifndef PONG_PRINT_H
#define PONG_PRINT_H

#include "pong.h"

void print_app(ball *b, field *f, player *p1, player *p2);	
void print_waiting_client(field *f);
void print_selecting_mode(field *f, player *p1, player *p2);
void print_app(ball *b, field *f, player *p1, player *p2);
void print_borders(int x_zero, int y_zero, int width, int height);
void print_player(int x, int top_y, int height);
void print_game (ball *b, field *f, player *p1, player *p2);
void print_connecting_server(field *f);

#endif //PONG_H