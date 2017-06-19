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
	if(gameEnd) return;
	bool isMyturn = player == turn;
	char msg[64];
	sprintf(msg, "Player #%d: %s", player > 0 ? 1 : 2, isMyturn ? "It's my turn" : "Waiting for peer");
	draw_message("                                 ", 0);
	draw_message(msg, !isMyturn/* color setting */);
	refresh();
}

int isValid(int x, int y, int player) {
	if(board[y][x]) return 0;
	int valid = 0;
	int otherPlayer = player * -1;
	/* check y = constance */
	for(int i = x + 1;i < 8;i++) {
		if((i == x + 1 && board[y][i] == player) || board[y][i] == 0) break;
		if(board[y][i] == otherPlayer) continue;
		else if(board[y][i] == player) {
			valid += 1;
			break;
		}
	}
	valid = valid << 1;
	for(int i = x - 1;i >= 0;i--) {
		if((i == x - 1 && board[y][i] == player) || board[y][i] == 0) break;
		if(board[y][i] == otherPlayer) continue;
		else if(board[y][i] == player) {
			valid += 1;
			break;
		}
	}
	valid = valid << 1;
	/* check x = constance */
	for(int i = y + 1;i < 8;i++) {
		if((i == y + 1 && board[i][x] == player) || board[i][x] == 0) break;
		if(board[i][x] == otherPlayer) continue;
		else if(board[i][x] == player) {
			valid += 1;
			break;
		}
	}
	valid = valid << 1;
	for(int i = y - 1;i >= 0;i--) {
		if((i == y - 1 && board[i][x] == player) || board[i][x] == 0) break;
		if(board[i][x] == otherPlayer) continue;
		else if(board[i][x] == player) {
			valid += 1;
			break;
		}
	}
	valid = valid << 1;
	/* check y = x + a */
	for(int i = x + 1, j = y + 1;i < 8 && j < 8;i++, j++) {
		if((i == x + 1 && board[j][i] == player) || board[j][i] == 0) break;
		if(board[j][i] == otherPlayer) continue;
		else if(board[j][i] == player) {
			valid += 1;
			break;
		}
	}
	valid = valid << 1;
	for(int i = x - 1, j = y - 1;i >= 0 && j >= 0;i--, j--) {
		if((i == x - 1 && board[j][i] == player) || board[j][i] == 0) break;
		if(board[j][i] == otherPlayer) continue;
		else if(board[j][i] == player) {
			valid += 1;
			break;
		}
	}
	valid = valid << 1;
	/* check y = -x + a */
	for(int i = x + 1, j = y - 1;i < 8 && j >= 0;i++, j--) {
		if((i == x + 1 && board[j][i] == player) || board[j][i] == 0) break;
		if(board[j][i] == otherPlayer) continue;
		else if(board[j][i] == player) {
			valid += 1;
			break;
		}
	}
	valid = valid << 1;
	for(int i = x - 1, j = y + 1;i >= 0 && j < 8;i--, j++) {
		if((i == x - 1 && board[j][i] == player) || board[j][i] == 0) break;
		if(board[j][i] == otherPlayer) continue;
		else if(board[j][i] == player) {
			valid += 1;
			break;
		}
	}
	return valid;
}

bool checkHasValid(int player) {
	bool has = false;
	for(int i = 0;i < 8;i++)
		for(int j = 0;j < 8;j++)
			if(isValid(i, j, player) > 0) {
				has = true;
				break;
			}
	return has;
}

void putPiece(int validStatus, int player) {
	int status = validStatus;
	board[cy][cx] = player;
	/* y = -x + a */
	if(status & 1) {
		for(int i = cx - 1, j = cy + 1;i >= 0 && j < 8;i--, j++) {
			if(board[j][i] == player) break;
			board[j][i] = player;
		}
	}
	status = status >> 1;
	if(status & 1) {
		for(int i = cx + 1, j = cy - 1;i < 8 && j >= 0;i++, j--) {
			if(board[j][i] == player) break;
			board[j][i] = player;
		}
	}
	status = status >> 1;
	/* y = x + a */
	if(status & 1) {
		for(int i = cx - 1, j = cy - 1;i >= 0 && j >= 0;i--, j--) {
			if(board[j][i] == player) break;
			board[j][i] = player;
		}
	}
	status = status >> 1;
	if(status & 1) {
		for(int i = cx + 1, j = cy + 1;i < 8 && j < 8;i++, j++) {
			if(board[j][i] == player) break;
			board[j][i] = player;
		}
	}
	status = status >> 1;
	/* check x = constance */
	if(status & 1) {
		for(int i = cy - 1;i >= 0;i--) {
			if(board[i][cx] == player) break;
			board[i][cx] = player;
		}
	}
	status = status >> 1;
	if(status & 1) {
		for(int i = cy + 1;i < 8;i++) {
			if(board[i][cx] == player) break;
			board[i][cx] = player;
		}
	}
	status = status >> 1;
	/* y = constance */
	if(status & 1) {
		for(int i = cx - 1;i >= 0;i--) {
			if(board[cy][i] == player) break;
			board[cy][i] = player;
		}
	}
	status = status >> 1;
	if(status & 1) {
		for(int i = cx + 1;i < 8;i++) {
			if(board[cy][i] == player) break;
			board[cy][i] = player;
		}
	}
	draw_board();
	draw_score();
	draw_cursor(cx, cy, 1);
	turn *= -1;
}

void judgeWhoWin(int player) {
	int black = 0, white = 0;
	for(int i = 0;i < BOARDSZ;i++) {
		for(int j = 0;j < BOARDSZ;j++) {
			if(board[i][j] == PLAYER1) white++;
			else if(board[i][j] == PLAYER2) black++;
		}
	}
	draw_message("                                      ", 0);
	if(white > black) draw_message("PLAYER 1 WIN!!!", player == PLAYER1);
	else if(white < black) draw_message("PLAYER 2 WIN!!!", player == PLAYER2);
	else draw_message("DRAW~~~", 0);
	refresh();
	turn = 0;
	gameEnd = true;
}

void reversi(int sock, int player) {
	init();
restart:
	resetBoard();
	while(true) {
		turnMsg(player);
		if(checkFd(STDIN_FILENO)) {
			int ch = getch(), validStatus;
			bool isMyturn = player == turn;

			switch(ch) {
			case ' ':
			case 0x0d:
			case 0x0a:
			case KEY_ENTER:
				validStatus = isValid(cx, cy, player);
				if(validStatus > 0 && isMyturn) {
					write(sock, " ", 1);
					putPiece(validStatus, player);
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
						putPiece(isValid(cx, cy, player * -1), player * -1);
						if(!checkHasValid(player)) {
							write(sock, "t", 1);
							turn *= -1;
						}
						break;
					case 't':
						if(!checkHasValid(player)) {
							write(sock, "e", 1);
							judgeWhoWin(player);
						}
						else turn *= -1;
						break;
					case 'e':
						judgeWhoWin(player);
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
