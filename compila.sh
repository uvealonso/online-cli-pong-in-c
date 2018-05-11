#!/bin/bash


gcc pong_check.c pong_print.c pong_do.c pong.c -std=gnu99 -lncurses -Wall -pedantic -o pong
