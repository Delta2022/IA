#include "../include/main.h"
#include "../include/menu_outputs.h"

// these place the vertical and horizontal seperator in the window
    // the position is the distance from 0, 0
#define V_SEP_POS COLS * 2 / 3
#define H_SEP_POS LINES * 2 / 3 - 1

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
    GRID_EDITOR grid_editor;
    WINDOW *grid_win;
    WINDOW *data_win;
    WINDOW *info_win;
    /*@unused@*/ WINDOW *p_note_win;

    WINDOW *v_sep;
    WINDOW *h_sep;
    WINDOW *detailed_data_win;
    WINDOW *commands_win;

    struct coord cursor = {0, 0};
    /*@null@*/ struct material *target_material = NULL;
    struct square *target_square;
    // ----- hide the cursor (its not needed)
    (void) curs_set(0);
    // NOTE newwin positions were found experimentally
        // using box and seeing the result when compiled
    // ----- init windows (numbers found mostly through trial and error)
    info_win = newwin(1, 0, 0, 0);
    grid_win = newwin(H_SEP_POS - 1, V_SEP_POS, 1, 0);
    data_win = newwin(0, 0, H_SEP_POS + 1, V_SEP_POS + 1);
    detailed_data_win = newwin(H_SEP_POS - 1, 0, 1, V_SEP_POS + 1);
    

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
    //(void) box(detailed_data_win, 0, 0);
    //(void) box(v_sep, 0, 0);
    //(void) box(h_sep, 0, 0);

    //(void) wnoutrefresh(stdscr);
    //(void) wnoutrefresh(info_win);
    //(void) wnoutrefresh(commands_win);
    //(void) wnoutrefresh(grid_win);
    //(void) wnoutrefresh(v_sep);
    //(void) wnoutrefresh(h_sep);
    //(void) wnoutrefresh(data_win);
    //(void) wnoutrefresh(detailed_data_win);
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


    // ----- put commands information

    while (true) {
        cursor = get_cursor(&grid_editor);
        target_square = &target_campaign->encounter_grid
                .squares[cursor.y][cursor.x];
        // ----- put info information
        (void) wmove(info_win, 0, 0);
        (void) wclrtoeol(info_win);
        (void) wattron(info_win, A_UNDERLINE);
        (void) mvwprintw(info_win, 0, 0, "Main Menu");
        (void) mvwprintw(info_win, 0, V_SEP_POS + 1, "cursor: (%d, %d)"
            , cursor.x, cursor.y);
        (void) wattroff(info_win, A_UNDERLINE);
        (void) mvwprintw(info_win, 0, V_SEP_POS, "|", cursor.x, cursor.y);
        // ----- update the data section
        (void) werase(data_win);
        mvwdisplay_square_info(data_win, 0, 0
            , target_square);

        // ----- put command information
        (void) werase(commands_win);
        (void) mvwprintw(commands_win, 0, 0, "Commands:");
        (void) mvwprintw(commands_win, 1, 0, "Arrow keys: move cursor");
        (void) mvwprintw(commands_win, 2, 0, "WASD: move grid around");

        (void) mvwprintw(commands_win, 3, 0, "o: move creatures around");
        (void) mvwprintw(commands_win, 4, 0, "m: edit creatures/materials");

        (void) mvwprintw(commands_win, 5, 0, "c: create new creature");
        (void) mvwprintw(commands_win, 6, 0, "i: create new item");
        (void) mvwprintw(commands_win, 7, 0, "m: create new material");

        (void) mvwprintw(commands_win, 0, 30, "Commands:");
        (void) mvwprintw(commands_win, 1, 30, "p: edit/create new positional"
            " note");
        (void) mvwprintw(commands_win, 2, 30, "r: repaint materials"
            " on grid");
        (void) mvwprintw(commands_win, 3, 30, "q: display campaign"
            " information");
        (void) mvwprintw(commands_win, 4, 30, "M: display material"
            " information at cursor");
        (void) mvwprintw(commands_win, 5, 30, "C: display creature"
            " information at cursor");
        (void) mvwprintw(commands_win, 6, 30, "P: display positional"
            " note at cursor");
        // ----- update the screen
        (void) wnoutrefresh(stdscr);
        (void) wnoutrefresh(info_win);
        (void) wnoutrefresh(commands_win);
        (void) wnoutrefresh(detailed_data_win);
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
            case 'm': // new material
                return_val = MATERIAL_CREATION;
                goto exit;
            case 'r': // repaint material
                return_val = REPAINT_MATERIALS;
                goto exit;
            case 'q': // campaign info
                mvwdisplay_campaign_info(detailed_data_win, 0, 0
                    , target_campaign);
                break;
            case 'o': // move creature
                return_val = MOVE_CREATURE;
                goto exit;
            case 'e': // edit positions
                return_val = EDIT_MENU;
                goto exit;
            case 'P':
                mvwdisplay_square_note_info(detailed_data_win, 0, 0
                    , target_square, cursor);
                break;
            case 'M': // material info
                target_material = target_square->material;
                mvwdisplay_material_info(detailed_data_win, 0, 0
                    , cursor, target_material);
                break;
            case 'C':
                // not great programming right now
                // print all characters
                (void) werase(detailed_data_win);
                if (target_square->num_creatures == 1) {
                    mvwdisplay_creature_info(detailed_data_win, 0, 0
                        , cursor, target_square->creatures[0]);
                    (void) wnoutrefresh(detailed_data_win);
                    (void) doupdate();
                    break;
                }

                list_square_creatures(detailed_data_win, 0, 0
                    , target_square);
                // update commands_win
                (void) werase(commands_win);
                (void) mvwprintw(commands_win, 0, 0, "Commands:");
                (void) wattron(commands_win, A_REVERSE);
                (void) mvwprintw(commands_win, 1, 0, "type a number"
                    " to select a character");
                (void) mvwprintw(commands_win, 2, 0, "type any other"
                    " character to skip");
                (void) wattroff(commands_win, A_REVERSE);
                (void) wnoutrefresh(commands_win);
                (void) wnoutrefresh(detailed_data_win);
                (void) doupdate();

                // get input
                c = getch();
                c -= (int) '0';
                if (c < 0 || c >= target_square->num_creatures) {
                    break;
                }
                mvwdisplay_creature_info(detailed_data_win, 0, 0
                    , cursor, target_square->creatures[c]);
        }
    }

exit:
    // ----- delete windows
    (void) delwin(info_win);
    (void) delwin(commands_win);
    (void) delwin(grid_win);
    (void) delwin(data_win);
    (void) delwin(detailed_data_win);
    (void) delwin(v_sep);
    (void) delwin(h_sep);
    
    // ----- clear screen
    (void) erase();
    (void) refresh();
    (void) curs_set(1);

    return return_val;
}


