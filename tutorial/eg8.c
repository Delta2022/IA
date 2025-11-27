#include <ncurses.h>

typedef struct _win_border_struct {
    chtype ls, rs, ts, bs, tl, tr, bl, br;
        // left_side, right_side, top_side, bottom_side
        // corners:
        // top_left, top_right, bottom_left, bottom_right

        // chtype is a character that also stores 
            // color information for ncurses
} WIN_BORDER;

typedef struct _WIN_struct {
    int startx, starty;
    int height, width;
    WIN_BORDER border;
} WIN;

void init_win_params(WIN *tar_win);
void print_win_params(WIN *tar_win);
void create_box(WIN *tar_win, bool draw_border);

int main(int argc, char *argv[])
{
    // this program doesn't use ncurses windows. it uses its own
        // preserves the same functionality though
    WIN win;
    int ch;

    initscr();
    start_color(); // start color functionality
    cbreak(); // remove buffering until \n

    keypad(stdscr, TRUE);

    noecho();

    init_pair(1, COLOR_CYAN, COLOR_BLACK);

    // initialize the window
    init_win_params(&win);
    print_win_params(&win);

    attron(COLOR_PAIR(1));
    printw("Press F1 to exit");
    attroff(COLOR_PAIR(1));
    
    create_box(&win, false);

    while((ch = getch()) != KEY_F(1)) {
        switch (ch) {
            case KEY_LEFT:
                create_box(&win, true);
                --win.startx;
                create_box(&win, false);
                break;

            case KEY_RIGHT:
                create_box(&win, true);
                ++win.startx;
                create_box(&win, false);
                break;

            case KEY_UP:
                create_box(&win, true);
                --win.starty;
                create_box(&win, false);
                break;

            case KEY_DOWN:
                create_box(&win, true);
                ++win.starty;
                create_box(&win, false);
                break;
        }
    }
    
    // end curses mode
    endwin();
    return 0;
}

void init_win_params(WIN *tar_win)
    // sets default values for a window
{
    tar_win->height = 3;
    tar_win->width = 10;
    tar_win->starty = (LINES - tar_win->height) / 2;
    tar_win->startx = (COLS - tar_win->width) / 2;

    tar_win->border.ls = '|';
    tar_win->border.rs = '|';
    tar_win->border.ts = '-';
    tar_win->border.bs = '-';
    tar_win->border.tl = '+';
    tar_win->border.tr = '+';
    tar_win->border.bl = '+';
    tar_win->border.br = '+';
}

void print_win_params(WIN *tar_win)
{
#ifdef _DEBUG
    mvprintw(LINES, 0, "%d %d %d %d", tar_win->startx, tar_win->starty
        , tar_win->width, tar_win->height);
    refresh();
#endif
}

void create_box(WIN *tar_win, bool draw_border)
    // creates a box where it is needed OR clears the box and anything
        // inside it
{
    int i, j;
    int x, y;
    int width, height;

    x = tar_win->startx;
    y = tar_win->starty;
    width = tar_win->width;
    height = tar_win->height;

    if (!draw_border) {
        // add the corners
        mvaddch(y, x, tar_win->border.tl);
        mvaddch(y, x + width, tar_win->border.tr);
        mvaddch(y + height, x, tar_win->border.br);
        mvaddch(y + height, x + width, tar_win->border.bl);

        // add the sides (+1 and -1's are to exclude the corners)
            // idk why this isn't -2 for height and width
        mvhline(y, x + 1, tar_win->border.ts, width - 1);
        mvhline(y + height, x + 1, tar_win->border.bs, width - 1);
        mvvline(y + 1, x, tar_win->border.ls, height - 1);
        mvvline(y + 1, x + width, tar_win->border.rs, height - 1);
    } else {
        // clears the box between x, y, width and height
        for (j = y; j <= y + height; j++) {
            for (i = x; i <= x + width; i++) {
                mvaddch(j, i, ' ');
            }
        }
    }

    refresh();
}
