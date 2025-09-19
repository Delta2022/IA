%: %.c
	gcc $@.c -o $@.out -lncurses
