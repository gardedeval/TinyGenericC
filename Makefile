INCLUDE = -Iinclude
SOURCE = \
	src/generic.c \
	src/json_deserialize.c \
	src/main.c
CC ?= gcc
FLAGS = -Wno-incompatible-pointer-types -std=c99
LIBS = -lm

.PHONY: build clean

build: 
	${CC} ${FLAGS} ${INCLUDE} ${SOURCE} ${LIBS} -o generic
	
clean: generic
	@rm generic
	