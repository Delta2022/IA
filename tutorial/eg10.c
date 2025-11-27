#include <stdio.h>
#include <ncurses.h>

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

int main (int argc, char *argv[])
{
    WINDOW *menu_win;
    int highlight = 0; // highlight goes from 0 to n_choices - 1
    int choice = -1;
    int c;
    int max_x = 0;
    int max_y = 0;
    int width = 0; // width and height of menu_win
    int height = 0;

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
        endwin();
        return 1;
    }

    startx = (max_x - width) / 2; // finds middle values
    starty = (max_y - height) / 2;

    menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, true); // allow special characters
    mvprintw(0, 0, "Use arrow keys to go up and down, press enter"
        " to select a choice");

    refresh();
    print_menu(menu_win, highlight);

    while (1) {
        c = wgetch(menu_win);

        switch (c) {
            case KEY_UP:
                if (highlight == 0) // wrap around from beginning to end
                    highlight = n_choices;
                else
                    highlight--;
                break;

            case KEY_DOWN:
                if (highlight == n_choices - 1) // wrap
                    highlight = 0;
                else
                    highlight++;
                break;

            case 10: // enter key
                choice = highlight;
                break;
            default:
                mvprintw(1, 0, "Character pressed is %3d, hopefully it"
                    " can be printed as '%c'", c, c);
                refresh();
                break;
        }
        print_menu(menu_win, highlight);

        if (choice != -1) { // user made a choice, so exit the loop
            break;
        }
    }

    // choice goes from 1 to n_choices 
        //but the array is 0 to n_choices - 1 so -1 is used
    mvprintw(max_y - 1, 0, "You choice %d ('%s')"
        , choice + 1, choices[choice]);

    getch();
    //clrtoeol(); // why? idk
    refresh();
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
