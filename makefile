# to run in terminal $ make

CC = gcc
#OPT = -O3
DBG = -g
WARN = -Wall
CFLAGS = $(DBG) $(WARN) $(INC) $(LIB)

# List all your .c files here (source files, excluding header files)
SRC_CODE = driver.c lex.c parser.c codegen.c vm.c

# List corresponding compiled object files here (.o files)
SRC_OBJ = driver.o lex.o parser.o codegen.o vm.o

#################################

# default rule

all: compile
	@echo "my work is done here..."


# rule for making compile

compile: $(SRC_OBJ)
	$(CC) -o compile $(CFLAGS) $(SRC_OBJ) -lm
	@echo "-----------DONE WITH COMPILE-----------"


# generic rule for converting any .c file to any .o file

.c.o:
	$(CC) $(CFLAGS)  -c $*.c


# type "make clean" to remove all .o files plus the src_prog binary

clean:
	rm -f *.o src_prog


# type "make clobber" to remove all .o files (leaves src_prog binary)

clobber:
	rm -f *.o
