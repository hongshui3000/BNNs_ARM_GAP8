CC = g++
CXX = gcc
CC_FLAGS = -O2 -std=c++11 -I.
CXX_FLAGES = -Wall

EXEC = main
SOURCES = $(wildcard *.cpp) $(wildcard layers/*.cpp) $(wildcard aquila/*.cpp)# $(wildcard aquila/*/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: clean

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@
#fft4g.o: fft4g.c
#	cc -c fft4g.c

clean:
	rm -f *~ *.o layers/*~ layers/*.o $(EXEC)
	#rm -f *~ *.o layers/*~ layers/*.o aquila/*~ aquila/*.o aquila/*/*~ aquila/*/*.o$(EXEC)
