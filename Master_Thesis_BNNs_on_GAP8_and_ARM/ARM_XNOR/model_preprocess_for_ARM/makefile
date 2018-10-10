CC = gcc
CC_FLAGS = -I. 

EXEC = conf
SOURCES = conf.c 
OBJECTS = $(SOURCES:.c=.o)

.PHONY: clean

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) -lm

%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f *~ *.o $(EXEC)