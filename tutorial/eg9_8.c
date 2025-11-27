#include <ncurses.h>

int main(int argc, char *argv[])
{
    int ch = 0;
    int y = 0;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);

    while (ch != KEY_F(1)) {
        printw("%d\n", ch);
        y++;
        if (y == LINES)
            clear();
        ch = getch();
    }

    endwin();
    return 0;
}
