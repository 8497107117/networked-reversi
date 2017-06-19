#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>

#include "othello.h"

using namespace std;

static int width;
static int height;
static int cx = 3;
static int cy = 3;
static int turn = PLAYER1;
static bool gameEnd = false;

void init();
void resetBoard();
bool checkFd(int fd);
void turnMsg(int player);
int isValid(int x, int y, int player);
bool checkHasValid(int player);
void putPiece(int validStatus, int player);
void judgeWhoWin(int player);
void reversi(int sock, int player);
