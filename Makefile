CXX = g++
CFLAGS = -Wall -g -I/usr/include/ncurses
LDFLAGS = -lncurses -pthread
TARGET = reversi
OBJECTS = main.o othello.o reversi.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CXX) -o $@ $^ $(LDFLAGS)
$(OBJECTS): %.o: %.cpp %.h
	$(CXX) -o $@ -c $< $(CFLAGS)

clean:
		rm -f *.o $(TARGET)
