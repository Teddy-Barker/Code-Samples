#Teddy Barker REDID: 825169794

#Makefile

#.RECIPEPREFIX +=

# Specify compiler
CC = g++
# Compiler flags, if you want debug info, add -g
CCFLAGS = -std=c++11 -g3 -Wall -c
CFLAGS = -g3 -c

# object files
OBJS = pageTable.o level.o tracereader.o main.o log.o tlb.o

# Program name
PROGRAM = pagingwithatc

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS)

main.o : main.cpp 
	$(CC) $(CCFLAGS) main.cpp

pageTable.o : pageTable.cpp pageTable.h 
	$(CC) $(CCFLAGS) pageTable.cpp

level.o :  level.cpp level.h
	$(CC) $(CCFLAGS) level.cpp
	
log.o : log.cpp log.h
	$(CC) $(CCFLAGS) log.cpp

tracereader.o : tracereader.cpp tracereader.h
	$(CC) $(CCFLAGS) tracereader.cpp

tlb.o : tlb.cpp tlb.h
	$(CC) $(CCFLAGS) tlb.cpp


# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -rf $(OBJS) *~ $(PROGRAM)