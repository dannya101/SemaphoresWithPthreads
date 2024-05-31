# CXX Make variable for compiler
CC=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CCFLAGS=-std=c++11 -Wall -g3 -c

# object files
OBJS = Shared.o main.o log.o doordash.o

# Program name
PROGRAM = fooddelivery

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -pthread -lrt -o $(PROGRAM) $(OBJS)

main.o : main.cpp Shared.h log.h fooddelivery.h doordash.h
	$(CC) $(CCFLAGS) main.cpp

Shared.o : Shared.cpp Shared.h
	$(CC) $(CCFLAGS) Shared.cpp

log.o : log.cpp log.h
	$(CC) $(CCFLAGS) log.cpp

doordash.o : doordash.cpp doordash.h
	$(CC) $(CCFLAGS) doordash.cpp

# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -f *.o *~ $(PROGRAM)
