#include "../include/main.h"

#define DEFAULT_MAIN_WIN_SPACE LINES * 3/4

int init_ui(/*@out@*/ struct ui_windows *target_ui_windows
    , int main_win_space)
    // creates the windows and writes a seperator into the seperator
    // window
{
    if (main_win_space <= 0) {
        main_win_space = DEFAULT_MAIN_WIN_SPACE;
    }
    target_ui_windows->info_win = newwin(1, 0, 0, 0);
    target_ui_windows->main_win = newwin(main_win_space, 0, 1, 0);
    target_ui_windows->seperator = newwin(1, 0, main_win_space + 1, 0);
    target_ui_windows->commands_win = newwin(0, 0, main_win_space + 2, 0);

    if (target_ui_windows->info_win == NULL) return -1;
    if (target_ui_windows->main_win == NULL) return -1;
    if (target_ui_windows->seperator == NULL) return -1;
    if (target_ui_windows->commands_win == NULL) return -1;


    (void) keypad(target_ui_windows->main_win, true);
    
    // write a seperating line over seperator
    for (int i = 0; i < COLS; i++) {
        (void) mvwprintw(target_ui_windows->seperator, 0, i, "-");
    }
    return 0;
}

void update_ui(struct ui_windows *target_ui_windows)
    // only writes to the virtual screen
    // requires running doupdate() after
{
    (void) wnoutrefresh(target_ui_windows->seperator);
    (void) wnoutrefresh(target_ui_windows->commands_win);
    (void) wnoutrefresh(target_ui_windows->info_win);
    (void) wnoutrefresh(target_ui_windows->main_win);
}

void del_ui(struct ui_windows *target_ui_windows)
{
    (void) delwin(target_ui_windows->info_win);
    (void) delwin(target_ui_windows->main_win);
    (void) delwin(target_ui_windows->seperator);
    (void) delwin(target_ui_windows->commands_win);
}
