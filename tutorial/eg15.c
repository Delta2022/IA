#include <panel.h>
#include <string.h>

#define NLINES 10
#define NCOLS 40


void print_in_middle(WINDOW *win, int starty, int startx
    , int width, char *string, chtype color);
void init_wins(WINDOW *wins[], int n);
void win_show(WINDOW *win, char *label, int label_color);

int main(int argc, char *argv[])
{
    WINDOW *my_wins[3];
    PANEL *my_panels[3];
    PANEL *top;
    int ch;

    // init
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, true);

    // init colors
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);

    init_wins(my_wins, 3);

    for (int i = 0; i < 3; i++) {
        my_panels[i] = new_panel(my_wins[i]);
    }
    // stdscr-0-1-2

    // set the user pointers (ig its just a space for us to store
        // a pointer???)
        // these are set in a loop 0 -> 1 -> 2 -> 0
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[2]);
    set_panel_userptr(my_panels[2], my_panels[0]);

    update_panels();

    // show info on screen
    attron(COLOR_PAIR(4));
    mvprintw(LINES - 2, 0, "Use tab to browse through windows "
        " (F1 = exit)");

    attroff(COLOR_PAIR(4));

    doupdate();

    top = my_panels[2];

    while((ch = getch()) != KEY_F(1)) {
        switch(ch) {
            case 9: // tab
                top = (PANEL *)panel_userptr(top); // use the
                    // user pointers to shift top by 1
                top_panel(top); // sets top as the panel on the top
                break;
        }
        update_panels();
        doupdate();
    }

    endwin();
    return 0;
}

void init_wins(WINDOW *wins[], int n)
    // put all the windows in their positions
{
    int x, y;
    char label[80];

    y = 2;
    x = 10;
    
    for (int i = 0; i < n; i++) {
        wins[i] = newwin(NLINES, NCOLS, y, x);
        sprintf(label, "Window #%d", i + 1);
        win_show(wins[i], label, i + 1);
        y += 3;
        x += 7;
    }
}

void win_show(WINDOW *win, char *label, int label_color)
{
    int startx, starty, height, width;

    getbegyx(win, starty, startx);
    getmaxyx(win, height, width);

    box(win, 0, 0);
    // write a horizontal line that merges with the box
    mvwaddch(win, 2, 0, ACS_LTEE);
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
    mvwaddch(win, 2, width - 1, ACS_RTEE);

    print_in_middle(win, 1, 0, width, label, COLOR_PAIR(label_color));
}

void print_in_middle(WINDOW *win, int starty, int startx
    , int width, char *string, chtype color)
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

    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
}
