FILES = main.c campaign.c grid.c note.c main.h menus.c creature.c get_multi_input.c grid_edit.c main_menu.c item.c

%: %.c
	gcc -gdwarf $@.c -o $@ -lpanel -lmenu -lncurses

main: $(FILES)
	gcc -gdwarf $(FILES) -o main -lpanel -lmenu -lncurses

splint:
	splint -incondefs +loopexec $(FILES)

ctags: $(FILES)
	ctags $(FILES)

test: test.c campaign.c grid.c note.c main.h
	gcc -gdwarf test.c campaign.c grid.c note.c -o test -lpanel -lmenu -lncurses

splint_test:
	splint -incondefs test.c campaign.c grid.c note.c
