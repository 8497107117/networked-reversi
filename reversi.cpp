#include "reversi.h"

void init() {
	initscr();			// start curses mode
	getmaxyx(stdscr, height, width);// get screen size

	cbreak();			// disable buffering
					// - use raw() to disable Ctrl-Z and Ctrl-C as well,
	halfdelay(1);			// non-blocking getch after n * 1/10 seconds
	noecho();			// disable echo
	keypad(stdscr, TRUE);		// enable function keys and arrow keys
	curs_set(0);			// hide the cursor

	init_colors();
}

void resetBoard() {
	clear();
	cx = cy = 3;
	turn = PLAYER1;
	init_board();
	draw_board();
	draw_cursor(cx, cy, 1);
	draw_score();
	refresh();

	attron(A_BOLD);
	move(height-1, 0);	printw("Arrow keys: move; Space/Return: put; R: reset; Q: quit");
	attroff(A_BOLD);
}

bool checkFd(int fd) {
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	select(fd + 1, &fds, NULL, NULL, &tv);
	return FD_ISSET(fd, &fds);
}

void turnMsg(int player) {
	bool isMyturn = player == turn;
	char msg[64];
	sprintf(msg, "Player #%d: %s", player > 0 ? 1 : 2, isMyturn ? "It's my turn" : "Waiting for peer");
	draw_message(msg, !isMyturn/* color setting */);
}

void reversi(int sock, int player) {
	init();
restart:
	resetBoard();
	while(true) {
		turnMsg(player);
		if(checkFd(STDIN_FILENO)) {
			int ch = getch();
			bool isMyturn = player == turn;

			switch(ch) {
			case ' ':
			case 0x0d:
			case 0x0a:
			case KEY_ENTER:
				if(!board[cy][cx] && isMyturn) {
					write(sock, " ", 1);
					board[cy][cx] = player;
					draw_cursor(cx, cy, 1);
					draw_score();
					turn = turn == PLAYER1 ? PLAYER2 : PLAYER1;
					turnMsg(player);
				}
				break;
			case 'q':
			case 'Q':
				write(sock, "q", 1);
				goto quit;
				break;
			case 'r':
			case 'R':
				write(sock, "r", 1);
				goto restart;
				break;
			case 'k':
			case KEY_UP:
				if(isMyturn) {
					write(sock, "k", 1);
					draw_cursor(cx, cy, 0);
					cy = (cy-1+BOARDSZ) % BOARDSZ;
					draw_cursor(cx, cy, 1);
				}
				break;
			case 'j':
			case KEY_DOWN:
				if(isMyturn) {
					write(sock, "j", 1);
					draw_cursor(cx, cy, 0);
					cy = (cy+1) % BOARDSZ;
					draw_cursor(cx, cy, 1);
				}
				break;
			case 'h':
			case KEY_LEFT:
				if(isMyturn) {
					write(sock, "h", 1);
					draw_cursor(cx, cy, 0);
					cx = (cx-1+BOARDSZ) % BOARDSZ;
					draw_cursor(cx, cy, 1);
				}
				break;
			case 'l':
			case KEY_RIGHT:
				if(isMyturn) {
					write(sock, "l", 1);
					draw_cursor(cx, cy, 0);
					cx = (cx+1) % BOARDSZ;
					draw_cursor(cx, cy, 1);
				}
				break;
			}
			refresh();
		}
		if(checkFd(sock)) {
			char buf[32];
			memset(buf, 0 ,sizeof(buf));
			int len = 0;
			while((len = read(sock, buf, 31)) != 0) {
				int ch = buf[0];
				switch(ch) {
					case ' ':
						board[cy][cx] = player == PLAYER1 ? PLAYER2 : PLAYER1;
						draw_cursor(cx, cy, 1);
						draw_score();
						turn = turn == PLAYER1 ? PLAYER2 : PLAYER1;
						turnMsg(player);
						break;
					case 'r':
						goto restart;
						break;
					case 'q':
						goto quit;
						break;
					case 'k':
						draw_cursor(cx, cy, 0);
						cy = (cy-1+BOARDSZ) % BOARDSZ;
						draw_cursor(cx, cy, 1);
						break;
					case 'j':
						draw_cursor(cx, cy, 0);
						cy = (cy+1) % BOARDSZ;
						draw_cursor(cx, cy, 1);
						break;
					case 'h':
						draw_cursor(cx, cy, 0);
						cx = (cx-1+BOARDSZ) % BOARDSZ;
						draw_cursor(cx, cy, 1);
						break;
					case 'l':
						draw_cursor(cx, cy, 0);
						cx = (cx+1) % BOARDSZ;
						draw_cursor(cx, cy, 1);
						break;
				}
				refresh();
				break;
			}
		}

		napms(1);		// sleep for 1ms
	}

quit:
	endwin();			// end curses mode

	return;
}
