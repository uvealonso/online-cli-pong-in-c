#ifndef PONG_DO_H
#define PONG_DO_H

#include "pong.h"

void do_game(ball *b, field *f, player *p1, player *p2);
void do_keyboard(int key, ball *b, field *f, player *p1, player *p2);
void do_network(ball *b, field *f, player *p1, player *p2, uint8_t *buffer, int buffer_size);

#endif //PONG_DO_H