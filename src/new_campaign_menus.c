#include "../include/main.h"

int start_campaign(/*@unused@*/ struct campaign *target_campaign)
    // campaign has to be initialized and defined
{
    char note_temp[MAX_CHAR];
    (void) memset(note_temp, 0, sizeof(note_temp));

    // get_multi_input argument values
    int text_pos[2] = {1, 3};
    int text_pos_len = ARRAY_LEN(text_pos);
    char *save_ptrs[2] = {target_campaign->name, note_temp};
    int max_lens[2] = {MAX_CHAR, MAX_CHAR};

    (void) clear(); // clear stdscr from the menu in start_menu
    (void) mvprintw(0, 0, "Name: ");
    (void) mvprintw(2, 0, "Notes: ");

    (void) mvprintw(LINES - 1, 0, "Press F1 to save values");
    (void) mvprintw(LINES - 2, 0, "Press F2 to move on");

    (void) get_multi_input(save_ptrs, text_pos_len
        , max_lens, text_pos);

    // TODO replace with note setter function when that is done
    strncpy(target_campaign->note.string, note_temp
        , (size_t) target_campaign->note.len);
    target_campaign->note.string[target_campaign->note.len - 1] = '\0';
    (void) getch();

    return 0;
}


int start_encounter(struct campaign *target_campaign)
    // TODO: allow support for large grids (scrolling)
    // returns -1 if menu is null
{
    struct grid *target_grid = &target_campaign->encounter_grid;
    struct material *mat_list = target_campaign->material_list;
    int mat_list_len = target_campaign->material_list_len;
    if (mat_list_len <= 0) {
        return -1;
    }
    // ----- init
    WINDOW *grid_win;
    WINDOW *menu_win;
    WINDOW *sub_win; // sub_win is the sub window of menu_win
    WINDOW *seperator;
    ITEM **menu_items;
    MENU *materials_menu;
    int half_length = COLS / 2; //TODO try making this a macro
    int c = -1;
    int cur_index = 0;
    ITEM *cur_item;
    int return_val = 0;

    struct coord grid_end = {0, 0};

    GRID_EDITOR grid_editor;
    
    // grid win is on the left and menu_win is on the right

    // even: (grid|sep|menu) __|_|_
    // odd: (grid|sep|menu) _|_|_
    grid_win = newwin(0, half_length, 0, 0);
    
    seperator = newwin(0, 1, 0, half_length);

    menu_win = newwin(0, 0, 0, half_length + 1);
    sub_win = derwin(menu_win, 0, 0, 1, 0);

    // ----- render a dividing line on the seperator window
    for (int i = 0; i < LINES; i++) {
        (void) mvwprintw(seperator, i, 0, "|");
    }

    // ----- print menu
    (void) mvwprintw(menu_win, 0, 0, "Material Options");

    // ----- print the squares
    getmaxyx(grid_win, grid_end.y, grid_end.x);
    (void) init_grid_editor(&grid_editor, target_grid
        , grid_end, grid_win);

    // ----- create menu for materials
    // define the item array
    menu_items = calloc((size_t) mat_list_len + 1, sizeof(*menu_items));

    if (menu_items == NULL) {
        return_val = -1;
        goto null_menu_exit;
    }

    // set the items
    for (int i = 0; i < mat_list_len; i++) {
        menu_items[i] = new_item(mat_list[i].name, mat_list[i].desc);
    }
    
    // define the menu
    materials_menu = new_menu((ITEM **)menu_items);
    if (materials_menu == NULL) goto null_menu_exit;

    (void) set_menu_win(materials_menu, menu_win);
    (void) set_menu_sub(materials_menu, sub_win);
    (void) post_menu(materials_menu);

    // ----- loop for inputs
    do {
        (void) wnoutrefresh(stdscr);
        (void) wnoutrefresh(grid_win);
        (void) wnoutrefresh(seperator);
        (void) wnoutrefresh(menu_win);
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

            case ' ': // set square to material

                // get the current selected material
                cur_item = current_item(materials_menu);
                if (cur_item == NULL) {
                    continue;
                }
                cur_index = item_index(cur_item);

                (void) grid_editor_driver(&grid_editor
                    , &mat_list[cur_index], SET_MAT);
                break;
            
            case 'n': // next material
                (void) menu_driver(materials_menu, REQ_DOWN_ITEM);
                break;
            case 'p': // previous material
                (void) menu_driver(materials_menu, REQ_UP_ITEM);
                break;
            case 'q': // toggle wall
                (void) grid_editor_driver(&grid_editor, NULL
                    , TOGGLE_WALL);
                break;

            // TODO: meke set wall and set material thing

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
        
    
    // ----- free menu itmes
    (void) unpost_menu(materials_menu);
    (void) free_menu(materials_menu);
null_menu_exit:
    for (int i = 0; i < mat_list_len; i++) {
        (void) free_item(menu_items[i]);
    }
    free(menu_items);

    // ----- free windows
    (void) delwin(grid_win);
    (void) delwin(sub_win); // sub_win must be freed first
    (void) delwin(menu_win);
    (void) delwin(seperator);

    // ----- clear the screen for the next function to use
    (void) erase();
    (void) refresh();

    return return_val;
}
