EXECUTABLE = Map
SOURCES = test.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CC = clang++
CFLAGS = -c -std=c++11 -g -Wall -Wextra -pedantic
LFLAGS = -ldl
RM = rm

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@  

clean:
	rm *.o
	$(RM) $(EXECUTABLE) 
