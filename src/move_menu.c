#include "../include/main.h"

void move_menu(struct campaign *target_campaign)
{
    // ----- initialize ui
    struct ui_windows ui;
    if (init_ui(&ui, LINES * 3/4 - 2) == -1) {
        return;
    }

    // ----- write values to info_win
    // info_win
    (void) wattron(ui.info_win, A_UNDERLINE);
    (void) mvwprintw(ui.info_win, 0, 0, "Move a Creature");
    (void) wattroff(ui.info_win, A_UNDERLINE);

    // ----- inits
    GRID_EDITOR grid_editor;
    struct coord grid_end;
    struct grid *target_grid = &target_campaign->encounter_grid;
    //struct coord source_pos = {0, 0};
    /*@null@*/ struct square *source_square = NULL;
    struct square *dest_square;
    //struct coord dest_pos = {0, 0};
    /*@null@*/ struct creature *target_creature = NULL;
    int temp_index = 0;
    int c = 0;

    struct coord cursor = {0, 0};
    struct square *cursor_square;

    // -----
    (void) curs_set(0);
    // ----- print squares
    getmaxyx(ui.main_win, grid_end.y, grid_end.x);
    (void) init_grid_editor(&grid_editor, target_grid
        , grid_end, ui.main_win, false);

    while (true) {
        // get important information from the grid
        cursor = get_cursor(&grid_editor);
        cursor_square = &target_campaign->encounter_grid.squares
            [cursor.y][cursor.x];
        // ----- commands_win
        (void) wclear(ui.commands_win);
        (void) mvwprintw(ui.commands_win, 0, 0, "Commands");
        (void) mvwprintw(ui.commands_win, 1, 0, "Arrow keys: move cursor");
        (void) mvwprintw(ui.commands_win, 2, 0, "WASD: move grid around");

        // only print space is available when there is a creature at
            // that square
        if (source_square == NULL) {
            if (cursor_square->num_creatures > 0)
                (void) mvwprintw(ui.commands_win, 3, 0, "space: select"
                    " creature at cursor to move around");
            else
                (void) mvwprintw(ui.commands_win, 3, 0, "there is no"
                    " creature at this square to move.");
        // source_square != NULL
        } else {
            (void) mvwprintw(ui.commands_win, 3, 0, "space: select"
                " square at cursor to move character to");
            (void) mvwprintw(ui.commands_win, 4, 0, "q: reset");
        }
            
        // ----- update screen
        (void) grid_editor_driver(&grid_editor, NULL, UPDATE_GRID);
        update_ui(&ui);
        (void) doupdate();

        c = wgetch(ui.main_win);
        // cursor movement
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

            // grid movement
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
            case 'q': // reset source_square
                if (source_square != NULL) {
                    source_square = NULL;
                }
                break;
            case ' ':
                // if this square is setting the source square
                if (source_square == NULL) {
                    source_square = &target_campaign->encounter_grid
                        .squares[cursor.y][cursor.x];

                    // check if square actually has creatures
                    if (source_square->num_creatures == 0) {
                        // reset
                        source_square = NULL;
                        break;
                    }
                    // find the correct creature (set target_creature)
                    if (source_square->num_creatures == 1) {
                        target_creature = source_square->creatures[0];
                    } else {
                        // prompt user
                        (void) wclear(ui.main_win);
                        list_square_creatures(ui.main_win, 0, 0
                            , source_square);
                        
                        // update commands_win to show new options
                        (void) wclear(ui.commands_win);
                        (void) mvwprintw(ui.commands_win, 0, 0
                            , "Commands");
                        (void) mvwprintw(ui.commands_win, 1, 0
                            , "Type a number listed to select"
                            " that character");
                        (void) mvwprintw(ui.commands_win, 2, 0
                            , "Type any other character to abort");
                        
                        (void) wnoutrefresh(ui.commands_win);
                        (void) wnoutrefresh(ui.main_win);
                        (void) doupdate();

                        // get an index from the user to use
                            c = getch();
                            c -= (int) '0';
                        if (c < 0 || c >= source_square->num_creatures) {
                            // reset
                            source_square = NULL;
                            break;
                        }
                        target_creature = source_square->creatures[c];
                    }
                    break;
                }

                // if this square is the destination square
                cursor = get_cursor(&grid_editor);
                dest_square = &target_campaign->encounter_grid.squares
                    [cursor.y][cursor.x];

                // ----- move the creature
                // add target_creature to dest_square
                if (dest_square->num_creatures
                    >= dest_square->max_creatures) {
                    // TODO show this
                    break;
                }
                dest_square->creatures[dest_square->num_creatures++]
                    = target_creature;

                // shift the creatures in source_square so it doesn't
                    // include the target_creature
                temp_index = 0;
                // num_creatures could have been used too
                for (int i = 0; i < source_square->max_creatures
                    ; i++) {
                    if (source_square->creatures[i] == target_creature) {
                        continue;
                    }
                    source_square->creatures[temp_index++]
                        = source_square->creatures[i];
                }
                source_square->num_creatures--;
                goto end;
            } 
    }

end:
    del_ui(&ui);
    (void) curs_set(1);
}
