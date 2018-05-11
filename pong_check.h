#ifndef PONG_CHECK_H
#define PONG_CHECK_H

#include "pong.h"

int check_ball_goal_new(field *f, ball *b);
int check_ball_borders_hit_new(ball *b, field *f);
int check_ball_players_hit_new(ball *b, player *p1, player *p2, int *dist_from_center);

#endif //PONG_H