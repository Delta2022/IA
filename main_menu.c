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

    // REMEMBER TO ERASE AND FREE WINDOWS at hte end
    int c = -1;
    struct coord grid_end;
    WINDOW *grid_win;
    GRID_EDITOR grid_editor;
    /*@unused@*/ WINDOW *data_win;
    /*@unused@*/ WINDOW *p_note_win;

    WINDOW *v_sep;
    WINDOW *h_sep;

    // ----- hide the cursor (its not needed)
    (void) curs_set(0);
    // NOTE newwin positions were found experimentally
        // using box and seeing the result when compiled
    // ----- init grid_win
    grid_win = newwin(H_SEP_POS, V_SEP_POS, 0, 0);

    getmaxyx(grid_win, grid_end.y, grid_end.x);
    (void) init_grid_editor(&grid_editor
        , &target_campaign->encounter_grid, grid_end, grid_win);

    // ----- init data_win
    data_win = newwin(H_SEP_POS, 0, 0, V_SEP_POS + 1);
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
        // ----- update the data section
        struct coord cursor = get_cursor(&grid_editor);
        (void) werase(data_win);
        mvdisplay_square_info(data_win, 0, 0
            , &target_campaign->encounter_grid
                .squares[cursor.y][cursor.x]);
        // ----- update the screen
        //box(grid_win, 0, 0);
        //box(v_sep, 0, 0);
        //box(h_sep, 0, 0);
        //box(data_win, 0, 0);

        (void) wnoutrefresh(stdscr);
        (void) wnoutrefresh(grid_win);
        (void) wnoutrefresh(v_sep);
        (void) wnoutrefresh(h_sep);
        (void) wnoutrefresh(data_win);
        (void) doupdate();

        // ----- process input
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

            case 'w': // move grid up
                (void) grid_editor_driver(&grid_editor, NULL, MOVE_UP);
                break;
            case 'd': // move grid left
                (void) grid_editor_driver(&grid_editor, NULL, MOVE_LEFT);
                break;
            case 'a': // move grid right
                (void) grid_editor_driver(&grid_editor, NULL, MOVE_RIGHT);
                break;
            case 's': // move grid down
                (void) grid_editor_driver(&grid_editor, NULL, MOVE_DOWN);
                break;
        }

    } while (c != KEY_F(2));

    // ----- delete windows
    (void) delwin(stdscr);
    (void) delwin(grid_win);
    (void) delwin(v_sep);
    (void) delwin(h_sep);
    (void) delwin(data_win);
    
    // ----- clear screen
    (void) erase();
    (void) refresh();

    return 0;
}


