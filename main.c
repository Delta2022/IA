#include "main.h"



int main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    // TODO define the function array
    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);
    // ----- start menu
    struct campaign temp;
    (void) init_campaign(&temp);
    (void) start_campaign(&temp);


    (void) endwin();
    debug_campaign(&temp, stdout);
    return 0;
}
