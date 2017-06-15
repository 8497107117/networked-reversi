#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <getopt.h>

using namespace std;

extern char *optarg;

void reaper(int sig);
