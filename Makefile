CC=gcc
FLAGS=-Wall -Werror  -Izdk -lcurses -lncurses -lm 
OBJECTS=zdk/*.o

jump_dude: jump_dude.c libzdk
	${CC} jump_dude.c  ${OBJECTS} ${FLAGS} -o $@

libzdk:
	make -C zdk

clean:
	rm jump_dude
