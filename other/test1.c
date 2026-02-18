#include <ncurses.h>

int main() {
    initscr();
    start_color();
    int c = 0;
    printf("has_colors(): %d, COLORS = %d, COLOR_PAIRS = %d"
        ", can_change_color = %d"
        , has_colors() ? 1 : 0, COLORS, COLOR_PAIRS
        , can_change_color() ? 1 : 0);
    for (int i = 0; i < 255; i++) {
        c = init_pair(i, i, COLOR_BLACK);
        attron(COLOR_PAIR(i));
        printw("a");
        attroff(COLOR_PAIR(i));
        
    }
    getch();

    endwin();
}
