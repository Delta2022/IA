#include <panel.h>
#include <string.h>
#include <stdlib.h>

typedef struct _PANEL_DATA {
    int x, y, w, h;
    char label[80];
    int label_color;
    PANEL *next;
} PANEL_DATA;

#define NLINES 10
#define NCOLS 40

void print_in_middle(WINDOW *win, int starty, int startx
    , int width, char *string, chtype color);
void win_show(WINDOW *win, char label[], int label_color);
void set_user_ptrs(PANEL *panels[], int n);
void init_wins(WINDOW *wins[], int n);

int main(int argc, char *argv[])
{
    WINDOW *my_wins[3];
    PANEL *my_panels[3];
    PANEL_DATA *top;
    PANEL *stack_top;
    WINDOW *temp_win, *old_win;
    int ch;
    int newx, newy, neww, newh; // what is neww and newh?
    bool size = false, move = false; // false?
    PANEL_DATA *stored_data = NULL;

    // ----- initalize
    (void) initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, true);

    // ----- initalize all the colors
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);

    init_wins(my_wins, 3);
        
    // attach a panel to each window
    my_panels[0] = new_panel(my_wins[0]);
    my_panels[1] = new_panel(my_wins[1]);
    my_panels[2] = new_panel(my_wins[2]);

    // set up user pointers
    set_user_ptrs(my_panels, 3);

    // the second panel should be on top
    update_panels(); // updates the virtual screen

    // show stuff on screen
    attron(COLOR_PAIR(4));
    mvprintw(LINES - 3, 0, "Use 'm' for moving, 'r' for resizing");
    mvprintw(LINES - 2, 0, "Use tab to browse through the windows"
        " (F1 to Exit)");
    attroff(COLOR_PAIR(4));
    doupdate(); // writes to the real screen

    stack_top = my_panels[2];

    // retrieve the PANEL_DATA informaiton for the top panel
    top = (PANEL_DATA *)panel_userptr(stack_top);
    newx = top->x;
    newy = top->y;
    neww = top->w;
    newh = top->h;

    while ((ch = getch()) != KEY_F(1))
    {
        switch (ch) {
            case 9: // tab -> switches the top panel
                top = (PANEL_DATA *)panel_userptr(stack_top);
                    // is this not already written?
                    // ig its here just in case
                top_panel(top->next);
                stack_top = top->next;
                top = (PANEL_DATA *)panel_userptr(stack_top);
                newx = top->x;
                newy = top->y;
                neww = top->w;
                newh = top->h;
                break;
            
            case 'r': // get into resize mode
                attron(COLOR_PAIR(4));
                mvprintw(LINES - 4, 0, "Resizing mode: use arrow"
                    " keys to resize, then press enter");
                refresh();
                attroff(COLOR_PAIR(4));
                size = true;
                break;

            case 'm': // enter move mode
                attron(COLOR_PAIR(4));
                mvprintw(LINES - 4, 0, "Enter mode: use arrow keys"
                    " to move, then press enter");
                refresh();
                attroff(COLOR_PAIR(4));
                move = true;
                break;
            case KEY_LEFT:
                if (size) { // expand the window left if size is true
                    newx--;
                    neww++;
                }
                if (move)
                    newx--;
                break;

            case KEY_RIGHT:
                if (size) { // reduce the window right
                    newx++;
                    neww--;
                }
                if (move)
                    newx++;
                break;
            case KEY_UP:
                if (size) { // reduce the window right
                    newy--;
                    newh++;
                }
                if (move)
                    newy--;
                break;
            case KEY_DOWN:
                if (size) { // reduce the window right
                    newy++;
                    newh--;
                }
                if (move)
                    newy++;
                break;
            case 10: // enter -> exit modes
                // clear the line saying that we're in resize/move mode
                move(LINES - 4, 0);
                clrtoeol();
                refresh();

                if (size) {
                    // resize by creating a new window
                    old_win = panel_window(stack_top);
                    temp_win = newwin(newh, neww, newy, newx);
                    replace_panel(stack_top, temp_win);
                    win_show(temp_win, top->label, top->label_color);
                    delwin(old_win);
                    size = false;
                }
                if (move) {
                    move_panel(stack_top, newy, newx);
                    move = false;
                }
                break;
        }
        // add info and update
        attron(COLOR_PAIR(4));
        mvprintw(LINES - 3, 0, "Use 'm' for moving, 'r' for resizing");
        mvprintw(LINES - 2, 0, "Use tab to browse through the windows"
            " (F1 to Exit)");
        attroff(COLOR_PAIR(4));
        refresh();
        update_panels();
        doupdate();
    }

    free(stored_data);
    endwin();
    return 0;
}

void init_wins(WINDOW *wins[], int n)
    // initalizes n winsows in the array wins
{
    int x, y;
    char label[80];
    y = 2;
    x = 10;

    for (int i = 0; i < n; i++) {
        wins[i] = newwin(NLINES, NCOLS, y, x);
        snprintf(label, 80, "Window Number %d", i + 1);
        win_show(wins[i], label, i + 1);
        y += 3;
        x += 7;
    }
}

void set_user_ptrs(PANEL *panels[], int n)
    // stores data about the panels into a PANEL_DATA
    // that is stored as part of the panel user pointer
{
    PANEL_DATA *ptrs; // is this freed ever?
    WINDOW *win;
    int x, y, w, h;
    char temp[80];

    ptrs = (PANEL_DATA *)calloc((size_t) n, sizeof(PANEL_DATA));

    for (int i = 0; i < n; i++) {
        // save the beginning x, y, width and length of window
        win = panel_window(panels[i]);
        getbegyx(win, y, x);
        getmaxyx(win, h, w);
        ptrs[i].x = x;
        ptrs[i].y = y;
        ptrs[i].w = w;
        ptrs[i].h = h;

        // save the label for the window
        snprintf(temp, 80, "Window Number %d", i + 1);
        strcpy(ptrs[i].label, temp);

        // assign a color group
        ptrs[i].label_color = i + 1;

        // save the next panel
        if (i + 1 == n)
            ptrs[i].next = panels[0];
        else
            ptrs[i].next = panels[i + 1];

        // save the user pointer
        set_panel_userptr(panels[i], &ptrs[i]);
    }
}

void win_show(WINDOW *win, char label[], int label_color)
    // shows a window with a border and some text (the label)
{
    int startx, starty, height, width;

    getbegyx(win, starty, startx);
    getmaxyx(win, height, width);

    box(win, 0, 0);
    
    // add a horizontal line for a label
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
    length = (int) strlen(string);
    temp = (width - length) / 2;
    x = startx + (int) temp;

    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
}
