FILES = main.c campaign.c grid.c note.c main.h start_menu.c

%: %.c
	gcc -gdwarf $@.c -o $@ -lpanel -lmenu -lncurses

main: $(FILES)
	gcc -gdwarf $(FILES) -o main -lpanel -lmenu -lncurses

splint:
	splint -incondefs $(FILES)

test: test.c campaign.c grid.c note.c main.h
	gcc -gdwarf test.c campaign.c grid.c note.c -o test -lpanel -lmenu -lncurses

splint_test:
	splint -incondefs test.c campaign.c grid.c note.c
