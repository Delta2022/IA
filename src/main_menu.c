#include "../include/main.h"
#include "../include/menu_outputs.h"

// these place the vertical and horizontal seperator in the window
    // the position is the distance from 0, 0
#define V_SEP_POS COLS * 2 / 3
#define H_SEP_POS LINES * 2 / 3

int main_menu(struct campaign *target_campaign)
    // does NOT use ui_v3
    // ----- returns -----
    // 0: exit
    // 1: creature creation
    // 2: p-note creation
    // 3: item creation
{
    // grid window
    // command list window
    // map window
    // p-note window
    // data window
    // vertical seperator
    // horizontal seperator

    // REMEMBER TO ERASE AND FREE WINDOWS at hte end
    int return_val = 0;
    int c = -1;
    struct coord grid_end;
    WINDOW *grid_win;
    GRID_EDITOR grid_editor;
    WINDOW *data_win;
    /*@unused@*/ WINDOW *p_note_win;

    WINDOW *v_sep;
    WINDOW *h_sep;
    WINDOW *info_win;
    WINDOW *commands_win;

    // ----- hide the cursor (its not needed)
    (void) curs_set(0);
    // NOTE newwin positions were found experimentally
        // using box and seeing the result when compiled
    // ----- init windows (numbers found mostly through trial and error)
    info_win = newwin(1, 0, 0, 0);
    grid_win = newwin(H_SEP_POS - 1, V_SEP_POS, 1, 0);
    data_win = newwin(H_SEP_POS - 1, 0, 1, V_SEP_POS + 1);

    commands_win = newwin(0, V_SEP_POS, H_SEP_POS + 1, 0);
    v_sep = newwin(0, 1, 1, V_SEP_POS);
    h_sep = newwin(1, 0, H_SEP_POS, 0);

    // ----- initialize grid editor 
    getmaxyx(grid_win, grid_end.y, grid_end.x);
    (void) init_grid_editor(&grid_editor
        , &target_campaign->encounter_grid, grid_end, grid_win, false);

    // check boundaries
    //(void) box(info_win, 0, 0);
    //(void) box(commands_win, 0, 0);
    //(void) box(grid_win, 0, 0);
    //(void) box(data_win, 0, 0);
    //(void) box(v_sep, 0, 0);
    //(void) box(h_sep, 0, 0);

    //(void) wnoutrefresh(stdscr);
    //(void) wnoutrefresh(info_win);
    //(void) wnoutrefresh(commands_win);
    //(void) wnoutrefresh(grid_win);
    //(void) wnoutrefresh(v_sep);
    //(void) wnoutrefresh(h_sep);
    //(void) wnoutrefresh(data_win);
    //(void) doupdate();
    //(void) getch();

    // ----- render dividing lines
    for (int i = 0; i < LINES; i++) {
        (void) mvwprintw(v_sep, i, 0, "|");
    }

    for (int i = 0; i < COLS; i++) {
        (void) mvwprintw(h_sep, 0, i, "-");
    }

    // -- draw an intersect point on top
        // (h_sep is updated after v_sep)
    (void) mvwprintw(h_sep, 0, V_SEP_POS, "+");

    // ----- put info information
    (void) wattron(info_win, A_UNDERLINE);
    (void) mvwprintw(info_win, 0, 0, "Main Menu");
    (void) wattroff(info_win, A_UNDERLINE);

    // ----- put commands information
    (void) mvwprintw(commands_win, 0, 0, "Movement Commands:");
    (void) mvwprintw(commands_win, 1, 0, "Arrow keys: move cursor");
    (void) mvwprintw(commands_win, 2, 0, "WASD: move grid around");

    (void) mvwprintw(commands_win, 0, 30, "Commands:");
    (void) mvwprintw(commands_win, 1, 30, "c: create new creature");
    (void) mvwprintw(commands_win, 2, 30, "p: create new positional note");
    (void) mvwprintw(commands_win, 3, 30, "i: create new item");
    (void) mvwprintw(commands_win, 4, 30, "m: create new material");
    (void) mvwprintw(commands_win, 5, 30, "M: repaint materials on grid");

    while (true) {
        // ----- update the data section
        struct coord cursor = get_cursor(&grid_editor);
        (void) werase(data_win);
        mvdisplay_square_info(data_win, 0, 0
            , &target_campaign->encounter_grid
                .squares[cursor.y][cursor.x]);
        // ----- update the screen
        (void) wnoutrefresh(stdscr);
        (void) wnoutrefresh(info_win);
        (void) wnoutrefresh(commands_win);
        (void) wnoutrefresh(grid_win);
        (void) wnoutrefresh(v_sep);
        (void) wnoutrefresh(h_sep);
        (void) wnoutrefresh(data_win);
        (void) doupdate();

        // ----- process input
        c = getch();
        if (c == KEY_F(2)) {
            return_val = 0;
            break;
        }
        switch (c) {
            case KEY_UP:
                (void) grid_editor_driver(&grid_editor, NULL
                    , CURSOR_UP);
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
                (void) grid_editor_driver(&grid_editor, NULL
                    , MOVE_LEFT);
                break;
            case 'a': // move grid right
                (void) grid_editor_driver(&grid_editor, NULL
                    , MOVE_RIGHT);
                break;
            case 's': // move grid down
                (void) grid_editor_driver(&grid_editor, NULL
                    , MOVE_DOWN);
                break;
            case 'c': // new creature
                return_val = CREATURE_CREATION;
                goto exit;
            case 'p': // new p-note
                return_val = P_NOTE_CREATION;
                goto exit;
            case 'i': // new item
                return_val = ITEM_CREATION;
                goto exit;
            case 'm':
                return_val = MATERIAL_CREATION;
                goto exit;
            case 'M':
                return_val = REPAINT_MATERIALS;
                goto exit;
            case 'q': // campaign info
                break;
        }
    }

exit:
    // ----- delete windows
    (void) delwin(grid_win);
    (void) delwin(v_sep);
    (void) delwin(h_sep);
    (void) delwin(data_win);
    
    // ----- clear screen
    (void) erase();
    (void) refresh();
    (void) curs_set(1);

    return return_val;
}


