FLAGS = -Wall -g
CC = gcc

all: gtk
HEADERS = proc.h
OBJECTS = proc.o  gtk.o

gtk : $(OBJECTS)
	$(CC) $(FLAGS) -o gtk $(OBJECTS) `pkg-config --cflags --libs gtk+-3.0`
%.o:%.c
	$(CC) $(FLAGS) -c $< `pkg-config --cflags --libs gtk+-3.0`

clean:
	rm *.o
