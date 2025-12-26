%: %.c
	gcc -gdwarf $@.c -o $@ -lpanel -lmenu -lncurses

main: main.c campaign.c grid.c note.c main.h
	gcc -gdwarf main.c campaign.c grid.c note.c -o main -lpanel -lmenu -lncurses

splint: main.c campaign.c grid.c note.c main.h
	splint -incondefs main.c campaign.c grid.c note.c
