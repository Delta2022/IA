// NOTE: seems to not work since I dont have a mouse??
#include <stdio.h>
#include <ncurses.h>
#include <string.h>

#define WIDTH 30
#define HEIGHT 10

// why are these global?
int startx = 0;
int starty = 0;

char *choices[] = {
    "Choice 1",
    "Choice 2",
    "Choice 3",
    "Choice 4",
    "Choice 5",
    "Exit"
};

int n_choices = sizeof(choices) / sizeof(char *);

void print_menu(WINDOW *menu_win, int highlight);
void report_choice(int mouse_x, int mouse_y, int *p_choice);

int main (int argc, char *argv[])
{
    WINDOW *menu_win;
    MEVENT event;
    int choice = -2; // ensures that until the user clicks an option
        // (and sets choice to an actual value)
        // choice is a value that prevents chosing an option
    int c;
    int max_x = 0;
    int max_y = 0;
    int width = 0; // width and height of menu_win
    int height = 0;
    int i = 0;
    int testx, testy;

    initscr();
    clear();
    noecho();
    cbreak(); // no buffering

    getmaxyx(stdscr, max_y, max_x); // gets max window size
        // in length

    width = 30;
    height = n_choices + 4; // allow space for each choice and
        // the outline and a space btwn. the outline and the list

    if (max_y < height || max_x < width) {
        mvprintw(0, 0, "Window too small");
        getch();
        goto end;
    }

    startx = (max_x - width) / 2; // finds middle values
    starty = (max_y - height) / 2;

    attron(A_REVERSE);
    mvprintw(max_y - 1, 0, "Click on exit to quit");
    attroff(A_REVERSE);
    refresh();

    menu_win = newwin(height, width, starty, startx);
    print_menu(menu_win, -1); // print with no highlight

    // get all mouse events
    //keypad(stdscr, true);
    keypad(menu_win, true); // THIS IS REQUIRED for wgetch(menu_win)
        // to output KEY_MOUSE
    mousemask(ALL_MOUSE_EVENTS, NULL);

    if (has_mouse() == false) {
        mvprintw(0, 0, "no mouse");
        getch();
        goto end;
    }

    while (1) {
        c = wgetch(menu_win);
        mvprintw(3, 0, "c is %d", c);

        switch (c) {
            case KEY_MOUSE: 
                mvprintw(1, 0, "key mouse event #%d", ++i);

                refresh();

                if (getmouse(&event) == OK) { // OK symbolizes that the
                    // event was inside the window

                    // NOTE: commented lines are a test of wmouse_trafo
                        // which seems to do nothing?
                    //testx = event.x;
                    //testy = event.y;

                    //wmouse_trafo(menu_win, &testx, &testy, true);
                    move(5, 0);
                    clrtoeol(); // clear the line
                    //printw("og x: %d, og y: %d, x: %d, y: %d"
                    //    , event.x, event.y, testx, testy);

                    printw("startx: %d, starty: %d"
                        ", x: %d, y: %d", startx, starty
                            , event.x, event.y);
                    refresh();

                    if (event.bstate & BUTTON1_CLICKED) {
                        // NOTE: PRESSED means held, whilst CLICKED
                            // means clicked
                        mvprintw(2, 0, "mouse pressed");
                        refresh();
                        report_choice(event.x, event.y
                            , &choice); // why + 1???
                        if (choice == -1) {// exit choice
                            goto end; //
                        } else if (choice != -2) { // check if
                            // choice is not its default value
                            mvprintw(max_y - 2, 1, "Choice made is: %d"
                                " ('%s')", choice + 1, choices[choice]);
                        }
                        refresh();
                    }
                }
                print_menu(menu_win, choice);
                break;

            case KEY_F(1):
                goto end;
                break;

            default:
                mvprintw(0, 0, "key %d pressed ('%c')", c, c);
                refresh();
        }
    }
    //clrtoeol(); // why? idk

end:
    endwin();
    return 0;
}

void print_menu(WINDOW *menu_win, int highlight)
    // highlight is the menu option to highlight out of the list
    // highlight goes 0 to n_choices - 1
{
    int x, y, i;

    x = 2;
    y = 2;

    box(menu_win, 0, 0);

    for (i = 0; i < n_choices; i++) {
        if (highlight == i) { // print option with highlight
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } else { // print option w/o highlight
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        }

        y++;
    }

    wrefresh(menu_win);
}

void report_choice(int mouse_x, int mouse_y, int *p_choice)
    // report a choice according to a given mouse position
    // TODO try using wmouse_trafo
{
    int offset_x, offset_y;

    offset_x = startx + 2; // these are from print_menu where
        // the printing starts at x = 2 and y = 2 relative to the window
    offset_y = starty + 2; // offset_x and offset_y convert
        // x and y positions relative to stdscr into values relative
        // to menu_win

    for (int choice = 0; choice < n_choices; choice++) {
        // tests if the mouse position is within the limits
        if (mouse_y == offset_y + choice
            && mouse_x >= offset_x
            && mouse_x < offset_x + strlen(choices[choice])) {
            
            if (choice == n_choices - 1) // exit option
                *p_choice = -1;
            else
                *p_choice = choice;
        }
    }
}
