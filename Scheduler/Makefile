#Teddy Barker REDID: 825169794

#Makefile

#.RECIPEPREFIX +=

# Specify compiler
CC = g++
# Compiler flags, if you want debug info, add -g
CCFLAGS = -std=c++11 -g3 -Wall -c
CFLAGS = -g3 -c

# object files
OBJS = main.o log.o scheduler.o

# Program name
PROGRAM = schedule

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS)

main.o : main.cpp 
	$(CC) $(CCFLAGS) main.cpp
	
log.o : log.cpp log.h
	$(CC) $(CCFLAGS) log.cpp

shared.o : scheduler.cpp scheduler.h
	$(CC) $(CCFLAGS) scheduler.cpp


# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -rf $(OBJS) *~ $(PROGRAM)