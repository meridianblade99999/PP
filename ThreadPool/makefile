CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=Main.cpp ThreadPool.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ThreadPool

all: $(SOURCES) $(EXECUTABLE)
clean:
	rm -f ThreadPool *.o

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@