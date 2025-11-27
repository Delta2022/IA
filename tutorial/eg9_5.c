#include <ncurses.h>

int main(int argc, char *argv[])
{
    initscr();

    if (can_change_color() == true)
        fputs("has cahnge colors\n", stdout);
    else 
        fputs("no change colors\n", stdout);

    endwin();
    return 0;
}
