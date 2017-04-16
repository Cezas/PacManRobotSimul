#Use of this file:
#	all:	will make entire project
#	clean:	removes everyting but the source files
#	remake:	cleans, then makes all

#This is the name of your executable program:
TARGET = pacman

#These are all the fancy flags
#	$^ all the dependancies
#	$< the first dependancy
#	$@ the target
FLAGS = -std=c++11 -fmax-errors=2 -Wall -o $@
CFLAGS = $(FLAGS) -c $<
LFLAGS = $(FLAGS) $^

#heirarchy of code:
# main
#   conio
# 	graph
# 		coordindex
# 		entities
# 			coordindex


all: $(TARGET)

$(TARGET):		main.o\
				graph.o\
				coordindex.o\
				entities.o\
#conio.o
	g++ $(LFLAGS)
main.o:			main.cpp\
				graph.o\
				coordindex.o\
				entities.o\
#conio.o
	g++ $(CFLAGS)
graph.o:		graph.cpp\
				graph.h\
				coordindex.o\
				entities.o
	g++ $(CFLAGS)
entities.o:		entities.cpp\
				entities.h\
				coordindex.o
	g++ $(CFLAGS)
coordindex.o:	coordindex.cpp\
				coordindex.h
	g++ $(CFLAGS)
#conio.o:        conio.cpp\
#				conio.h
#	g++ $(CFLAGS)


clean:
	rm -f *.o $(TARGET)
remake: clean all
