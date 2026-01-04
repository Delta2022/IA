#include "main.h"

// these place the vertical and horizontal seperator in the window
    // the position is the distance from 0, 0
#define V_SEP_POS COLS * 2 / 3
#define H_SEP_POS LINES * 2 / 3

int main_menu(struct campaign *target_campaign)
{
    // grid window
    // command list window
    // map window
    // p-note window
    // data window
    // vertical seperator
    // horizontal seperator

    int c = -1;
    struct coord grid_end;
    WINDOW *grid_win;
    GRID_EDITOR grid_editor;
    /*@unused@*/ WINDOW *data_win;
    /*@unused@*/ WINDOW *p_note_win;

    WINDOW *v_sep;
    WINDOW *h_sep;

    // ----- init grid_win
    grid_win = newwin(H_SEP_POS, V_SEP_POS, 0, 0);

    getmaxyx(grid_win, grid_end.y, grid_end.x);
    (void) init_grid_editor(&grid_editor
        , &target_campaign->encounter_grid, grid_end, grid_win);
    // ----- init seperator windows
    // NOTE: for the intersect point, render h_sep above v_sep
    v_sep = newwin(0, 1, 0, V_SEP_POS);
    h_sep = newwin(1, 0, H_SEP_POS, 0);

    // ----- render dividing lines
    for (int i = 0; i < LINES; i++) {
        (void) mvwprintw(v_sep, i, 0, "|");
    }

    for (int i = 0; i < COLS; i++) {
        (void) mvwprintw(h_sep, 0, i, "-");
    }

    // -- draw an intersect point
    (void) mvwprintw(h_sep, 0, V_SEP_POS, "+");

    do {
        // ----- show data in the data window
        // TODO
        // ----- update the screen
        (void) wnoutrefresh(stdscr);
        (void) wnoutrefresh(grid_win);
        (void) wnoutrefresh(v_sep);
        (void) wnoutrefresh(h_sep);
        (void) doupdate();

        // --- process input
        c = getch();
        switch (c) {
            case KEY_UP:
                (void) grid_editor_driver(&grid_editor, NULL, CURSOR_UP);
                break;
            case KEY_DOWN:
                (void) grid_editor_driver(&grid_editor, NULL
                    , CURSOR_DOWN);
                break;
            case KEY_LEFT:
                (void) grid_editor_driver(&grid_editor, NULL
                    , CURSOR_LEFT);
                break;
            case KEY_RIGHT:
                (void) grid_editor_driver(&grid_editor, NULL
                    , CURSOR_RIGHT);
                break;

            case 's': // move grid up
                (void) grid_editor_driver(&grid_editor, NULL, MOVE_UP);
                break;
            case 'a': // move grid left
                (void) grid_editor_driver(&grid_editor, NULL, MOVE_LEFT);
                break;
            case 'd': // move grid right
                (void) grid_editor_driver(&grid_editor, NULL, MOVE_RIGHT);
                break;
            case 'w': // move grid down
                (void) grid_editor_driver(&grid_editor, NULL, MOVE_DOWN);
                break;
        }
    } while (c != KEY_F(2));
    return 0;
}
