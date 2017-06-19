#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>

#include "othello.h"

static int width;
static int height;
static int cx = 3;
static int cy = 3;
static int turn = PLAYER1;

void init();
void resetBoard();
bool checkFd(int fd);
void turnMsg(int player);
void reversi(int sock, int player);
