#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <getopt.h>

#include "othello.h"

#define QLEN 32

using namespace std;

extern char *optarg;

static int width;
static int height;
static int cx = 3;
static int cy = 3;

void reversi(int sock);
int passiveSocket(const int port, const char *transport, int qlen);
void server(int port);
void client(const char *des);
