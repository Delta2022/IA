#include <ncurses.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char mesg[] = "Just a string";
    int row = 0;
    int col = 0;
    
    initscr();
    getmaxyx(stdscr, row, col); // macro, so no pointers
    mvprintw(row / 2, (col - strlen(mesg)) / 2, "%s", mesg);
    mvprintw(row - 2, 0, "This screen has %d rows and %d columns\n"
        , row, col);
    printw("Try resizing your window and then run this program again");
    refresh();
    getch();
    endwin();

    return 0;
}
