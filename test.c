#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

int main(int argc, char *argv[])
{
        initscr();
        printw("Hello World!\n");
        refresh();
        getch();
        endwin();
        return 0;
}
