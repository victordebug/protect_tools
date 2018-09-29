DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

#VPATH = ./src:./include:./obj:./bin

CC = gcc
CFLAGS =-Wall
INCLUDES = -I${DIR_INC}
TARGET = protect_tool

SRC = $(wildcard ${DIR_SRC}/*.c)
OBJ = $(patsubst %.c, ${DIR_OBJ}/%.o, $(notdir $(SRC)))
BIN_TARGET = ${DIR_BIN}/${TARGET}

#make target
${BIN_TARGET}:${OBJ}
	${CC} ${OBJ} -o $@

#make clean
.PHONY : clean
clean:
	rm -rf  ${DIR_OBJ}/*.o ${BIN_TARGET}

#dependence
${DIR_OBJ}/%.o: ${DIR_SRC}/%.c
	${CC} ${CFLAGS} ${INCLUDES} -c $< -o $@