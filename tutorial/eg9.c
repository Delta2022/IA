#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

void print_in_middle(WINDOW *win, int starty, int startx
    , int width, char *string);

int main(int argc, char *argv[])
{
    initscr();
    
    if (has_colors() == FALSE) {
        endwin(); // exit ncurses
        printf("Your terminal does not support color\n"); // print
            // regularly
        exit(1);
    }

    start_color(); // start colors
    init_pair(1, COLOR_RED, COLOR_BLACK);

    attron(COLOR_PAIR(1));
    print_in_middle(stdscr, LINES / 2, 0, COLS, "Viola!!! In color...");

    getch();
    endwin();

    
    return 0;
}

void print_in_middle(WINDOW *win, int starty, int startx
    , int width, char *string)
    // prints in the middle of the next section of width characters
    // if starty and/or startx are zero, the relevant values from
    // the current cursor are used instead
    // width defaults to 80
{
    int length, x, y = 0;
    float temp = 0;

    if (win == NULL)
        win = stdscr;

    // use the values given for startx & starty unless they are zero,
        // then use the current values
        // allows for flexibility
    getyx(win, y, x);
    if (startx > 0)
        x = startx;
    if (starty > 0)
        y = starty;
            
    // give a default for width if not given
    if (width <= 0)
        width = 80;

    // calculate the correct starting position for printing the string
    length = strlen(string);
    temp = (width - length) / 2;
    x = startx + (int) temp;

    mvwprintw(win, y, x, "%s", string);
    refresh();
}
