# ------------------------------------------------
# Makefile
# ------------------------------------------------

# Executable name
EXEC_NAME = run

# Compiler to use
CC = g++

# Default Options to use
# CFLAGS = -Wall -Wno-write-strings -O0 -g
CFLAGS = -Wno-write-strings -O3 -g
# Source files
SRC =	Graph.cpp \
	main.cpp

OBJ = ${SRC:.cpp=.o}

#------------------------------------------------------------

all: ${EXEC_NAME}

${EXEC_NAME}: ${OBJ}
	${CC} ${CFLAGS} -o ${EXEC_NAME} ${OBJ}

%.o: %.cpp
	${CC} ${CFLAGS} -c -o $@ $+

clean:
	rm ${EXEC_NAME} *.o *~ *# -rf
