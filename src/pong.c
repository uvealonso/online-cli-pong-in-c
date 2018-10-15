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
#include <netdb.h>
 
#include "pong_do.h"
#include "pong_print.h"
#include "pong_check.h"


int sockfd = 0;
struct sockaddr_in sockdir;
struct sockaddr_in sockclient;

void initialize_screen() {
	srand(time(NULL));
	initscr();
	curs_set(0);
	keypad(stdscr, TRUE);

	//initiate color function
	start_color();
	bkgd(COLOR_PAIR(1));
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	timeout(0);

	//multiple color pairs to use
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_CYAN, COLOR_BLUE);		
	init_pair(2, COLOR_BLACK, COLOR_BLUE);		
	init_pair(3, COLOR_WHITE, COLOR_RED);
	init_pair(4, COLOR_BLUE, COLOR_WHITE);
	init_pair(5, COLOR_RED, COLOR_WHITE);
	init_pair(6, COLOR_BLACK, COLOR_CYAN);
	init_pair(7, COLOR_BLUE, COLOR_WHITE);
}


void set_player (player *p, int x, int top_y, int slice_height, int score, char *name) {
	p->x = x;
	p->top_y = top_y;
	p->slice_height = slice_height;
	p->score = score;
	p->mode_selected = 0;
	p->mode_confirmed = 0;
	strcpy(p->name, name);
}


void set_field (field *f, int left_offset, int top_offset, int width, int height) {
	f->left_offset = left_offset;
	f->top_offset = top_offset;
	f->width = width;
	f->height = height;
}



void move_player (int16_t *player_top_y, int slice_height, int top_border, int bottom_border, int position_increment) {
	*player_top_y += position_increment;

	if (*player_top_y <= top_border) {
		*player_top_y = top_border + 1;
		return;
	}

	if (*player_top_y + slice_height > bottom_border) {
		*player_top_y = bottom_border - slice_height;
		return;
	}
}

void send_network_message(int type, ball *b, field *f, player *p1, player *p2)
{	
	const int sockclientlen = sizeof(struct sockaddr_in);   
	char buffer[512];
	int msg_len = 0;
	switch (type) {
		case MESSAGE_WELCOME_TO_SRV:
			buffer[0] = type;
			strncpy(buffer + 1, p1->name, PLAYER_NAME_MAX_LEN);
			msg_len = strlen(p1->name) + 1;
			break;

		case MESSAGE_WELCOME_TO_CLI:
			buffer[0] = type;
			buffer[1] = f->width;
			buffer[2] = f->height;
			strncpy(buffer + 3, p1->name, PLAYER_NAME_MAX_LEN);
			msg_len = strlen(p1->name) + 3;
			break;

		case MESSAGE_MENU_SEL:
			buffer[0] = type;
			buffer[1] = p1->mode_selected;
			msg_len = 2;
			break;

		case MESSAGE_MENU_SEL_CNFRM:
			buffer[0] = type;
			msg_len = 1;
			break;

		case MESSAGE_MENU_SEL_NCNFRM:
			buffer[0] = type;
			msg_len = 1;
			break;

		case MESSAGE_BALL_POS:
			buffer[0] = type;
			buffer[1] = (b->x_th & 0xFF00u) >> 8;
			buffer[2] = (b->x_th & 0x00FFu);
			buffer[3] = (b->y_th & 0xFF00u) >> 8;
			buffer[4] = (b->y_th & 0x00FFu);
			msg_len = 5;
			break;

		case MESSAGE_PLAYER_POS:
			buffer[0] = type;
			buffer[1] = (p1->x & 0xFF00u) >> 8;
			buffer[2] = (p1->x & 0x00FFu);
			buffer[3] = (p1->top_y & 0xFF00u) >> 8;
			buffer[4] = (p1->top_y & 0x00FFu);
			msg_len = 5;
			break;
	}
	
	sendto(sockfd,
		buffer,
		msg_len,
		0,
		(struct sockaddr*) &sockclient,
		sockclientlen);
 
}


int main(int argc, char **argv)
{
	// Poll variables
	int retPoll = 0;
	struct pollfd *pfd;
	// Timer variables
	int timerfd;
	struct itimerspec it;
	// Socket variables
	//int sockfd = 0;
	//struct sockaddr_in sockdir;
	//struct sockaddr_in sockclient;

	int be_host = 0;

	/* Tamaño de la estructura anterior */     
	int sockclientlen = sizeof(struct sockaddr_in);   

	uint8_t buffer[BUFLEN];   

	int ch;

	player p1;
	player p2;
	ball b;
	field f;

	set_player(&p1, SLICE_GOAL_SEPARATION, 6, SLICE_HEIGHT, 0, "Player 1");
	set_player(&p2, (SCENERY_WIDTH - 1) - SLICE_GOAL_SEPARATION, 6, SLICE_HEIGHT, 0, "Player 2");
	b.x_th = 24000;
	b.y_th = 5000;
	b.x_speed_th = -1000;
	b.y_speed_th = 0;
	set_field(&f, SCENERY_LEFT_OFFSET, SCENERY_TOP_OFFSET, SCENERY_WIDTH, SCENERY_HEIGHT);
	f.game_status = STATUS_SELECT_MODE;


	if (argc > 1) {
		strncpy(p1.name, argv[1], PLAYER_NAME_MAX_LEN);
	}

	if (argc < 3)
	{
		be_host = 1;
	}

	strcpy(f.debug, "");

	/* create timer */
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_nsec = 30 * 1000 * 1000;
	it.it_value.tv_sec = 0;
	it.it_value.tv_nsec = 30 * 1000 * 1000;
	timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if (timerfd_settime(timerfd, 0, &it, NULL) != 0) {
		return -1;
	}

	memset(&sockdir, 0, sizeof(struct sockaddr_in));
	memset(&sockclient, 0, sizeof(struct sockaddr_in));



	sockfd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if (be_host) {
		sockdir.sin_family = AF_INET;
		sockdir.sin_port = htons(PORT);
		sockdir.sin_addr.s_addr = htonl(INADDR_ANY);
		if (sockfd == 0) {
			perror("sockfd");
			exit(1);
		}
		if (bind (sockfd, (struct sockaddr *)&sockdir, sizeof(sockdir)) == -1){
			perror("bind");
			exit(1);
		}
		f.game_status = STATUS_WAITING_CLIENT;
	} else {	
		sockclient.sin_family = AF_INET;
		sockclient.sin_port = htons(PORT);
		inet_aton(argv[2] , &sockclient.sin_addr);
		f.game_status = STATUS_CONNECTING_SERVER;
		send_network_message(MESSAGE_WELCOME_TO_SRV, &b, &f, &p1, &p2);
	}

	pfd = (struct pollfd *) malloc(sizeof(struct pollfd) * (3)); // timer and stdin
	pfd[0].fd = fileno(stdin);
	pfd[0].events = POLLIN;
	pfd[1].fd = sockfd;
	pfd[1].events = POLLIN;
	pfd[2].fd = timerfd;
	pfd[2].events = POLLIN;


	initialize_screen();
	print_app(&b, &f, &p1, &p2);

	for (;;)
	{
		int pollEvents = be_host ? 3 : 2;
		pollEvents = (f.game_status == STATUS_PLAYING) ? pollEvents : 2;
		retPoll = poll(pfd, pollEvents, -1);

		if (retPoll == -1) {
			exit(EXIT_FAILURE);
		} else if (retPoll == 0) {
			continue;
		}
		
		//POLL STDIN
		if ((pfd[0].revents & POLLIN) == POLLIN) {
			ch = getch();
			do_keyboard(ch, &b, &f, &p1, &p2);
			print_app(&b, &f, &p1, &p2);
		}


		//POLL SOCKET
		if ((pfd[1].revents & POLLIN) == POLLIN) {
			int recbytes = recvfrom (sockfd,
				&buffer,
				BUFLEN,
				0,
				(struct sockaddr *)&sockclient,
				(socklen_t *)&sockclientlen);

			do_network(&b, &f, &p1, &p2, buffer, recbytes);
			print_app(&b, &f, &p1, &p2);
		}

		//POLL TIMER
		if ((pfd[2].revents & POLLIN) == POLLIN) {
			uint64_t expirations;
			read(timerfd, &expirations, sizeof(uint64_t));
			
			do_game(&b, &f, &p1, &p2);
			print_app(&b, &f, &p1, &p2);
			send_network_message(MESSAGE_BALL_POS, &b, &f, &p1, &p2);
		}

		//refresh();
	}//while (!quit) bracket. Do not delete this brace

	endwin();

}//Closing bracket of main function
