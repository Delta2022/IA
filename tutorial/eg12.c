#include <ncurses.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    initscr();
    printw("Hello World!!!\n");
    refresh();
    getch();

    def_prog_mode(); // save the ncurses stuff
    endwin(); // exit ncurses

    system("/bin/sh"); // cooked mode lmao

    reset_prog_mode();
    refresh();

    printw("Another string\n");
    refresh();
    getch();
    endwin();

    return 0;
}
