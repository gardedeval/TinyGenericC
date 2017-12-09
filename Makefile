INCLUDE = -Iinclude
SOURCE = \
	src/main.c
CC ?= gcc 
FLAGS = -std=c99 
LIBS = -lm 

.PHONY: build clean

build: 
	${CC} ${FLAGS} -DDEBUG ${INCLUDE} ${SOURCE} ${LIBS} -o generic
	
optimized:
	${CC} ${FLAGS} -O2 -DNDEBUG ${INCLUDE} ${SOURCE} ${LIBS} -o generic
	
valgrind: generic
	valgrind ./generic
	
clean: generic
	@rm generic
	