#include <panel.h>

int main(int argc, char *argv[])
{
    WINDOW *my_wins[3];
    PANEL *my_panels[3];
    int lines = 10, cols = 40, y = 2, x = 4;

    initscr();
    cbreak();
    noecho();

    // create windows for the panels
    my_wins[0] = newwin(lines, cols, y, x);
    my_wins[1] = newwin(lines, cols, y + 1, x + 5);
    my_wins[2] = newwin(lines, cols, y + 2, x + 10);

    // create borders for the windows
    for (int i = 0; i < 3; i++) {
        box(my_wins[i], 0, 0);
    }

    // attach a panel to each window
        // NOTE: this works as a stack
    for (int i = 0; i < 3; i++) {
        my_panels[i] = new_panel(my_wins[i]);
    }
    // should be in the order stdscr-0-1-2

    // update
    update_panels();

    // refresh screen
    doupdate();

    getch();
    endwin();
}
