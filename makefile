# The variable CC specifies which compiler will be used.
# (because different unix systems may use different compilers)
CC=gcc

# The variable CFLAGS specifies compiler options
#   -c :    Only compile (don't link)
#   -Wall:  Enable all warnings about lazy / dangerous C programming 
#   -std=c99: Using newer C99 version of C programming language
CFLAGS=-c -Wall -std=c99 -Wextra -O1 -g -pthread

# linker flags
CLINKFLAGS =-lpthread -lm

# All of the .h header files to use as dependencies
HEADERS=vector.h histogram.h parallel_helpers.h return_code.h config.h

# All of the object files to produce as intermediary work
OBJECTS=main.o vector.o histogram.o parallel_helpers.o

# The final program to build
EXECUTABLE=histo_program.out

# --------------------------------------------

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CLINKFLAGS) $(OBJECTS) -o $(EXECUTABLE)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o $(EXECUTABLE)
