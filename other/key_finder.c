#include <stdio.h>
#include <ncurses.h>

int main()
{
    int c = -1;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);

    while ((c = getch()) != KEY_F(1)) {
        printw("%d\n", c);
    }

    endwin();
}
