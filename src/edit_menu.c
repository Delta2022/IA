#include "../include/main.h"

static void clean_ui(struct ui_windows *ui);
static void reinit_ui(struct ui_windows *ui, GRID_EDITOR *grid_editor);

void edit_menu(struct campaign *target_campaign)
// TODO convert material and campaign creation menus
// then add to here
// then also do updating the ui after key input since it doesn't update at all lol
{
    // ----- initialize ui
    struct ui_windows ui;
    if (init_ui(&ui, LINES - 8) == -1) {
        return;
    }
    
    // prep for the GRID_EDITOR
    (void) curs_set(0);

    // ----- inits
    GRID_EDITOR grid_editor;
    struct coord grid_end;
    struct grid *target_grid = &target_campaign->encounter_grid;
    int c = 0;
    struct coord cursor;
    struct square *cursor_square;
    /*@null@*/ struct creature *target_creature = NULL;

    // ----- print squares/init GRID_EDITOR
    getmaxyx(ui.main_win, grid_end.y, grid_end.x);
    (void) init_grid_editor(&grid_editor, target_grid
        , grid_end, ui.main_win, false);

    while (true) {
        // get useful values
        cursor = get_cursor(&grid_editor);
        cursor_square = &target_campaign->encounter_grid.squares
            [cursor.y][cursor.x];

        // info_win
        (void) wattron(ui.info_win, A_UNDERLINE);
        (void) mvwprintw(ui.info_win, 0, 0, "Edit Data");
        (void) wattroff(ui.info_win, A_UNDERLINE);
        // commands_win
        (void) wclear(ui.commands_win);
        (void) mvwprintw(ui.commands_win, 0, 0, "Commands");
        (void) mvwprintw(ui.commands_win, 1, 0
            , "Arrow keys: move cursor");
        (void) mvwprintw(ui.commands_win, 2, 0, "WASD: move grid around");

        if (cursor_square->num_creatures == 0)
            (void) mvwprintw(ui.commands_win, 3, 0, "there are no"
                " creatures at the cursor");
        else
            (void) mvwprintw(ui.commands_win, 3, 0, "c: edit creature"
                " at cursor");
        if (cursor_square->material == NULL)
            (void) mvwprintw(ui.commands_win, 4, 0, "there is no"
                " material at the cursor");
        else
            (void) mvwprintw(ui.commands_win, 4, 0, "m: edit material"
                " at cursor");
        (void) mvwprintw(ui.commands_win, 5, 0, "p: edit/create"
            " positional note at cursor");

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
            case 'c': // select creature to edit
                // locate creature
                if (cursor_square->num_creatures == 0) {
                    break;
                } else if (cursor_square->num_creatures == 1) {
                    target_creature = cursor_square->creatures[0];
                } else { // more than one creature
                    (void) wclear(ui.main_win);
                    list_square_creatures(ui.main_win, 0, 0
                        , cursor_square);
                    // update user
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

                    
                    c = wgetch(ui.main_win);
                    c -= (int) '0';

                    if (c < 0 || c >= cursor_square->num_creatures) {
                        break;
                    }

                    target_creature = cursor_square->creatures[c];
                }
                
                // clean up ui for creature_creation_menu
                clean_ui(&ui);
                // edit creature information
                (void) creature_creation_menu(target_campaign
                    , target_creature, true);

                reinit_ui(&ui, &grid_editor);

                break;
            case 'm': // select material to edit
                if (cursor_square->material == NULL)
                    break;

                clean_ui(&ui);
                material_creation_menu(target_campaign, cursor_square
                    ->material);
                reinit_ui(&ui, &grid_editor);
                break;
            case 'q': // select campaign to edit
                clean_ui(&ui);
                (void) start_campaign(target_campaign);
                reinit_ui(&ui, &grid_editor);
                break;
            case 'p':
                // search through the list of p_notes for one with
                    // the correct position
                clean_ui(&ui);
                // NOTE: cursor is not used in p_note_creation_menu
                (void) square_note_creation_menu(target_campaign, cursor);
                reinit_ui(&ui, &grid_editor);

                break;
            case KEY_F(2):
                goto exit;
        }
    }

exit:
    // ----- remove ui
    del_ui(&ui);
    (void) curs_set(1);
}

static void clean_ui(struct ui_windows *ui)
{
    del_ui(ui);
    (void) curs_set(1);
}

static void reinit_ui(struct ui_windows *ui, GRID_EDITOR *grid_editor)
{
    (void) curs_set(0);
    if (init_ui(ui, 0) == -1) {
        return;
    }
    // change the grid_editor's window to the new window
    (void) change_window(grid_editor, ui->main_win);
}
