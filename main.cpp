#include "main.h"

int main(int argc, char **argv) {
	if(argc != 3) {
		cout << "wrong argument count" << endl;
		return 0;
	}
	int ch;
	while((ch = getopt(argc, argv, "s:c:")) != -1) {
		switch(ch) {
			case 's':
				cout << "server " << optarg << endl;
				break;
			case 'c':
				cout << "client " << optarg << endl;
				break;
			case '?':
				cout << "Unknown option." << endl;
				break;
		}
	}
}
