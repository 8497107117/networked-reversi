#include "main.h"

void reversi(int sock) {
	initscr();			// start curses mode
	getmaxyx(stdscr, height, width);// get screen size

	cbreak();			// disable buffering
					// - use raw() to disable Ctrl-Z and Ctrl-C as well,
	halfdelay(1);			// non-blocking getch after n * 1/10 seconds
	noecho();			// disable echo
	keypad(stdscr, TRUE);		// enable function keys and arrow keys
	curs_set(0);			// hide the cursor

	init_colors();

restart:
	clear();
	cx = cy = 3;
	init_board();
	draw_board();
	draw_cursor(cx, cy, 1);
	draw_score();
	refresh();

	attron(A_BOLD);
	move(height-1, 0);	printw("Arrow keys: move; Space: put GREEN; Return: put PURPLE; R: reset; Q: quit");
	attroff(A_BOLD);

	while(true) {			// main loop
		int ch = getch();
		int moved = 0;

		switch(ch) {
		case ' ':
			board[cy][cx] = PLAYER1;
			draw_cursor(cx, cy, 1);
			draw_score();
			break;
		case 0x0d:
		case 0x0a:
		case KEY_ENTER:
			board[cy][cx] = PLAYER2;
			draw_cursor(cx, cy, 1);
			draw_score();
			break;
		case 'q':
		case 'Q':
			goto quit;
			break;
		case 'r':
		case 'R':
			goto restart;
			break;
		case 'k':
		case KEY_UP:
			draw_cursor(cx, cy, 0);
			cy = (cy-1+BOARDSZ) % BOARDSZ;
			draw_cursor(cx, cy, 1);
			moved++;
			break;
		case 'j':
		case KEY_DOWN:
			draw_cursor(cx, cy, 0);
			cy = (cy+1) % BOARDSZ;
			draw_cursor(cx, cy, 1);
			moved++;
			break;
		case 'h':
		case KEY_LEFT:
			draw_cursor(cx, cy, 0);
			cx = (cx-1+BOARDSZ) % BOARDSZ;
			draw_cursor(cx, cy, 1);
			moved++;
			break;
		case 'l':
		case KEY_RIGHT:
			draw_cursor(cx, cy, 0);
			cx = (cx+1) % BOARDSZ;
			draw_cursor(cx, cy, 1);
			moved++;
			break;
		}

		if(moved) {
			refresh();
			moved = 0;
		}

		napms(1);		// sleep for 1ms
	}

quit:
	endwin();			// end curses mode

	return;
}

int passiveSocket(const int port, const char *transport, int qlen) {
	struct protoent *ppe; /* pointer to protocol information entry*/
	struct sockaddr_in serverInfo; /* an Internet endpoint address */
	int sock, type; /* socket descriptor and socket type */

	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr = INADDR_ANY;
	serverInfo.sin_port = htons((unsigned short)port);
	/* Map protocol name to protocol number */
	if((ppe = getprotobyname(transport)) == 0) {
		printf("can't get \"%s\" protocol entry\n", transport);
		exit(1);
	}
	/* Use protocol to choose a socket type */
	if(!strcmp(transport, "udp"))
		type = SOCK_DGRAM;
	else if(!strcmp(transport, "tcp"))
		type = SOCK_STREAM;
	/* Allocate a socket */
	sock = socket(PF_INET, type, ppe->p_proto);
	if(sock < 0) {
		printf("can't create socket\n");
		exit(1);
	}
	/* Bind the socket */
	if(bind(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) < 0) {
		printf("can't bind to %d ports\n", port);
		exit(1);
	}
	/* Listen */
	if(type == SOCK_STREAM && listen(sock, qlen) < 0) {
		printf("can't listen on %d port\n", port);
		exit(1);
	}
	else
		printf("Waiting for a client on port %d...\n", port);

	return sock;
}

void server(int port) {
	struct sockaddr_in clientInfo; /* the address of a client */
	unsigned int clientAddrLen; /* length of client's address */
	int masterSock; /* master server socket */
	int slaveSock; /* slave server socket */
	masterSock = passiveSocket(port, "tcp", QLEN);

	clientAddrLen = sizeof(clientInfo);
	slaveSock = accept(masterSock, (struct sockaddr *)&clientInfo, &clientAddrLen);
	if(slaveSock < 0) { cout << "accept error" << endl; exit(1); }
	reversi(slaveSock);
	close(slaveSock);
	return;
}

void client(const char *des) {
	string tmp(des);
	int pos = tmp.find(":");
	const char *ip = tmp.substr(0, pos).c_str();
	int port = atoi(tmp.substr(pos + 1).c_str());
	struct sockaddr_in serverInfo; /* the address of a server */
	int sock;
	sock = socket(PF_INET, SOCK_STREAM, 0);
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons((unsigned short)port);
	inet_pton(AF_INET, ip, &serverInfo.sin_addr.s_addr);
	if(connect(sock, (struct sockaddr *) &serverInfo, sizeof(serverInfo)) < 0) {
		printf("connect fail on port: %d\n", port);
		return;
	}
	reversi(sock);
	close(sock);
	return;
}

int main(int argc, char **argv) {
	if(argc != 3) {
		cout << "wrong argument count" << endl;
		return 0;
	}
	int ch, port;
	while((ch = getopt(argc, argv, "s:c:")) != -1) {
		switch(ch) {
			case 's':
				port = atoi(optarg);
				server(port);
				exit(0);
				break;
			case 'c':
				client(optarg);
				exit(0);
				break;
			case '?':
				cout << "Unknown option." << endl;
				break;
		}
	}
}
