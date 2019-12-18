TARGET = feasibility
LIBS = -lm -lgomp
CC = g++

CFLAGS = -g -fopenmp -std=c++11 -Ioriginal -Istd -Iabstract 

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -g -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
