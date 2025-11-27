// splint +charindex -incondefs eg17.c

#include <panel.h>
#include <string.h>
#include <stdlib.h>

typedef struct _PANEL_DATA {
    int hide; // true if panel is hidden
} PANEL_DATA;

static void init_wins(/*@partial@*/ WINDOW **wins, int n);
WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
static void print_in_middle(WINDOW *win, int starty, int startx
    , int width, char *string, chtype color);
static void win_show(WINDOW *win, char *label, int label_color);

// redefine ncurses methods for splint
extern int wattr_on(WINDOW *win, attr_t attrs, /*@null@*/ void *opts);
extern int wattr_off(WINDOW *win, attr_t attrs, /*@null@*/ void *opts);
extern /*@temp@*/ WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
extern /*@dependent@*/ const void *panel_userptr(const PANEL *pan);
extern int endwin(void);
extern int set_panel_userptr(PANEL *pan, /*@null@*/ const void *ptr);

#define NLINES 10
#define NCOLS 40

int main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    WINDOW *my_wins[3];
    PANEL *my_panels[3];
    PANEL_DATA *panel_datas = calloc(3, sizeof(panel_datas));
        // NOTE: calloc was used here to stop a 'data isn't defined'
        // warning for splint, but using memset probably
        // would have fixed this as well
    PANEL_DATA *temp;
    int ch;
    int target_window = 0;

    if (panel_datas == NULL) {
        exit(EXIT_FAILURE);
    }

    // ----- initialize curses
    (void) initscr();
    (void) start_color();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);

    // ----- initalize all the colors
    (void) init_pair(1, COLOR_RED, COLOR_BLACK);
    (void) init_pair(2, COLOR_GREEN, COLOR_BLACK);
    (void) init_pair(3, COLOR_BLUE, COLOR_BLACK);
    (void) init_pair(4, COLOR_CYAN, COLOR_BLACK);

    init_wins(my_wins, 3);

    // attach a panel to each window
    my_panels[0] = new_panel(my_wins[0]);
    my_panels[1] = new_panel(my_wins[1]);
    my_panels[2] = new_panel(my_wins[2]); // bottom up order, so
        // stdscr -> 0 -> 1 -> 2

    // initalize panel_datas and say that all panels are unhidden
    for (int i = 0; i < 3; i++) {
        panel_datas[i].hide = false;
    }

    // store panel_data into the panel userptr
    for (int i = 0; i < 3; i++) {
        (void) set_panel_userptr(my_panels[i], &panel_datas[i]);
    }

    // update the stacking order
    update_panels();

    // show initial information on screen
    (void) attron(COLOR_PAIR(4));
    (void) mvprintw(LINES - 3, 0, "Show or hide a window with 'a' "
        ", 'b', or 'c'");
    (void) mvprintw(LINES - 2, 0, "F1 to Exit");
    (void) attroff(COLOR_PAIR(4));
    
    (void) doupdate();

    // get user input
    while ((ch = getch()) != KEY_F(1)) {
        switch (ch) {
            case 'a':
                target_window = 0;
                break;
            case 'b':
                target_window = 1;
                break;
            case 'c':
                target_window = 2;
                break;
        }

        temp = (PANEL_DATA *)panel_userptr(my_panels[target_window]);
        if (temp->hide == 0) {
            (void) hide_panel(my_panels[target_window]);
            temp->hide = 1;
        } else {
            (void) show_panel(my_panels[target_window]);
            temp->hide = 0;
        }
        (void) update_panels();
        (void) doupdate();
    }

    // release the references to panel_datas so this program can free it
        // because splint is complaining that im not freeing it
        // even though endwin does
    for (int i = 0; i < 3; i++) {
        (void) set_panel_userptr(my_panels[i], NULL);
    }

    free(panel_datas);
    
    (void) endwin();

    return 0;
}

static void init_wins(WINDOW *wins[], int n)
    // initalize the window (NOTE: wins is reldef)
{
    int x = 10;
    int y = 2;
    char label[80];

    for (int i = 0; i < n; i++) {
        wins[i] = newwin(NLINES, NCOLS, y, x);
        if (wins[i] == NULL) {
            exit(EXIT_FAILURE);
        }
        (void) snprintf(label, 80, "Window number %d", i + 1);
        win_show(wins[i], label, i + 1);
        y += 3;
        x += 7;
    }
}

static void win_show(WINDOW *win, char *label, int label_color)
{
    int startx, starty, height, width;

    getbegyx(win, starty, startx);
    getmaxyx(win, height, width);

    // outline the window
    (void) box(win, 0, 0);

    // make a horizontal line or something
    mvwaddch(win, 2, 0, ACS_LTEE);
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
    mvwaddch(win, 2, width - 1, ACS_RTEE);

    print_in_middle(win, 1, 0, width, label, COLOR_PAIR(label_color));
}

static void print_in_middle(WINDOW *win, int starty, int startx
    , int width, char *string, chtype color)
    // prints a line in the middle
{
    int length, x, y;
    double temp;

    if(win == NULL)
            win = stdscr;
    getyx(win, y, x);
    if(startx != 0)
        x = startx;
    if(starty != 0)
        y = starty;
    if(width == 0)
        width = 80;

    length = (int) strlen(string);
    temp = (double) (width - length) / 2;
    x = startx + (int) temp;
    (void) wattron(win, color);
    (void) mvwprintw(win, y, x, "%s", string);
    (void) wattroff(win, color);
    (void) refresh();
}
