INCLUDE = -Iinclude
SOURCE = \
	src/generic.c \
	src/json_deserialize.c \
	src/main.c
CC ?= gcc 
FLAGS = -std=c99 
LIBS = -lm 

.PHONY: build clean

build: 
	${CC} ${FLAGS} ${INCLUDE} ${SOURCE} ${LIBS} -o generic
	
optimized:
	${CC} ${FLAGS} -O2 ${INCLUDE} ${SOURCE} ${LIBS} -o generic
	
clean: generic
	@rm generic
	