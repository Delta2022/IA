// crudely moves around windows
#include <ncurses.h>

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

int main(int argc, char *argv[])
{
    WINDOW *my_win;
    int height = 3;
    int width = 10;
    int starty, startx = 0;
    int ch = 0;
    int y, x = 0;

    initscr();
    cbreak();

    // NOTE: the use of LINES and COLS has to happen after initscr
    // ALSO NOTE: LINES and COLS are max index already
    starty = (LINES - height) / 2; // default starty&x to a
        // center placement
    startx = (COLS - width) / 2;
    keypad(stdscr, TRUE);

    printw("Press F1 to exit");
    refresh();

    my_win = create_newwin(height, width, starty, startx);

    while((ch = getch()) != KEY_F(1)) {
        switch(ch) {
            case KEY_LEFT:
                // delete existing and make a new window that is shifted
                
                // the if statement tests if the window is in bounds
                if (startx - 1 >= 0) {
                    destroy_win(my_win);
                    my_win = create_newwin(height, width
                        , starty, --startx);
                }
                break;

            case KEY_RIGHT:
                if (startx + 1 + width <= COLS) { // both parts are
                    // max index so its fine
                    destroy_win(my_win);
                    my_win = create_newwin(height, width
                        , starty, ++startx);
                }
                break;

            case KEY_UP:
                if (starty - 1 >= 0) {
                    destroy_win(my_win);
                    my_win = create_newwin(height, width
                        , --starty, startx);
                }
                break;

            case KEY_DOWN:
                if (starty + 1 + height <= LINES) {
                    destroy_win(my_win);
                    my_win = create_newwin(height, width
                        , ++starty, startx);
                }
                break;

        }
    }
    delwin(my_win);
    endwin();
    return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);

    wrefresh(local_win);

    return local_win;
}

void destroy_win(WINDOW *local_win)
{
    // NOTE that box(local_win, ' ', ' ') doesn't work since the corners
        // are left behind

    // clears the borders of the window
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
 	 // 1. win: the window on which to operate
         // the following are the characters for:
	 // 2. ls: left side of the window 
	 // 3. rs: right side of the window 
	 // 4. ts: top side of the window 
	 // 5. bs: bottom side of the window 
	 // 6. tl: top left corner of the window 
	 // 7. tr: top right corner of the window 
	 // 8. bl: bottom left corner of the window 
	 // 9. br: bottom right corner of the window

         wrefresh(local_win);
         delwin(local_win);
}
