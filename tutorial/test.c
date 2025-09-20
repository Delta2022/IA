#include <ncurses.h>

int main(int argc, char *argv[])
{
    initscr();
    addch('a' | A_REVERSE | A_DIM);
    getch();
    endwin();
    return 0;
}
