########################################
##
## Makefile
## LINUX compilation 
##
##############################################





#FLAGS
C++FLAG = -g -std=c++14

MATH_LIBS = -lm

EXEC_DIR=.


.cc.o:
	g++ $(C++FLAG) $(INCLUDES)  -c $< -o $@
.cpp.o:
	g++ $(C++FLAG) $(INCLUDES) -c $< -o $@



#Including
INCLUDES=  -I. 

#-->All libraries (without LEDA)
LIBS_ALL =  -L/usr/lib -L/usr/local/lib 


#Setting up attributes for programs


ALL_OBJ1=p1.o DisjSets.o image.o 
ALL_OBJ2=p2.o DisjSets.o image.o
ALL_OBJ3=p3.o DisjSets.o image.o
ALL_OBJ4=p4.o DisjSets.o image.o


PROGRAM_1 = p1
PROGRAM_2 = p2
PROGRAM_3 = p3
PROGRAM_4 = p4



$(PROGRAM_1): $(ALL_OBJ1)
	g++ $(C++FLAG) -o $(EXEC_DIR)/$@ $(ALL_OBJ1) $(INCLUDES) $(LIBS_ALL)
$(PROGRAM_2): $(ALL_OBJ2)
	g++ $(C++FLAG) -o $(EXEC_DIR)/$@ $(ALL_OBJ2) $(INCLUDES) $(LIBS_ALL)
$(PROGRAM_3): $(ALL_OBJ3)
	g++ $(C++FLAG) -o $(EXEC_DIR)/$@ $(ALL_OBJ3) $(INCLUDES) $(LIBS_ALL)
$(PROGRAM_4): $(ALL_OBJ4)
	g++ $(C++FLAG) -o $(EXEC_DIR)/$@ $(ALL_OBJ4) $(INCLUDES) $(LIBS_ALL)


all: 

	make $(PROGRAM_1)
	make $(PROGRAM_2)
	make $(PROGRAM_3)
	make $(PROGRAM_4)

run_p1: 	
		./$(PROGRAM_1) two_objects.pgm p1_results_two_objects.pgm
run_p2: 	
		./$(PROGRAM_2) p1_results_two_objects.pgm p2_results_two_objects.pgm
run_p3: 	
		./$(PROGRAM_3) p2_results_two_objects.pgm object_database.txt p3_results_two_objects.pgm
run_p4: 	
		./$(PROGRAM_4) many_objects_2.pgm object_database.txt p4_results_two_objects.pgm




clean:
	(rm -f *.o;)

(:
