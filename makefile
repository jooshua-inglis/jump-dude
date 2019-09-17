CC=gcc
FLAGS=-Wall -Werror  -Izdk -lncurses -lm
OBJECTS=zdk/*.o

jump_dude: jump_dude.c jump_dude.h
	${CC} ${FLAGS} ${OBJECTS} jump_dude.c -o $@


zdk: 
	make -C zdk/makefile 


clean:
	rm jump_dude
