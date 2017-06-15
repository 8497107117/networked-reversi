CXX = g++
CFLAGS = -g -Wall
TARGET = othello
OBJECTS = main.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CXX) -o $@ $^
$(OBJECTS): %.o: %.cpp %.h
	$(CXX) -o $@ -c $< $(CFLAGS)

clean:
		rm -f *.o othello
