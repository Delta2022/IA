#include "../include/main.h"

#define MAIN_WIN_SPACE LINES * 3/4
int start_campaign(struct campaign *target_campaign)
    // campaign has to be initialized and defined
    // prompts the user for inputs regarding the campaign
{
    // ----- initialize windows
    struct ui_windows ui;
    if (init_ui(&ui, 0) == -1) {
        return -1;
    }
    
    // ----- write values to ui.info_win and ui.commands_win
    // info win
    (void) wattron(ui.info_win, A_UNDERLINE);
    (void) mvwprintw(ui.info_win, 0, 0, "Campaign Information");
    (void) wattroff(ui.info_win, A_UNDERLINE);

    // commands
    (void) mvwprintw(ui.commands_win, 0, 0, "Commands");
    (void) mvwprintw(ui.commands_win, 1, 0, "Up arrow/down arrow:"
        " traverse through input fields");
    (void) mvwprintw(ui.commands_win, 2, 0, "F2: submit information");
    (void) mvwprintw(ui.commands_win, 3, 0, "Note: only characters"
        " a-z and A-Z are allowed.");

    // ----- update screen
    update_ui(&ui);
    (void) doupdate();
    // ----- get input from user
    //char note_temp[MAX_CHAR];
    //(void) memset(note_temp, 0, sizeof(note_temp));

    // get_multi_input argument values
    int text_pos[2] = {1, 3};
    int text_pos_len = ARRAY_LEN(text_pos);
    char *save_ptrs[2] = {target_campaign->name
        , target_campaign->note.string};
    int max_lens[2] = {MAX_CHAR, MAX_CHAR};

    (void) clear(); // clear stdscr from the menu in start_menu
    (void) mvwprintw(ui.main_win, 0, 0, "Name: ");
    (void) mvwprintw(ui.main_win, 2, 0, "Notes: ");

    (void) get_multi_input(ui.main_win, save_ptrs, text_pos_len
        , max_lens, text_pos);

    // TODO replace with note setter function when that is done
    //strncpy(target_campaign->note.string, note_temp
    //    , (size_t) target_campaign->note.len);
    //target_campaign->note.string[target_campaign->note.len - 1] = '\0';

    del_ui(&ui);
    
    (void) erase();
    return 0;
}


int start_encounter(struct campaign *target_campaign)
    // TODO: allow support for large grids (scrolling)
    // paints the grid with materials
    // returns -1 if menu is null
    // returns -2 if ui cant be created (newwin failed)
{
    // ----- initialize ui
    struct ui_windows ui;
    if (init_ui(&ui, LINES * 3/4 - 2) == -1) {
        return -2;
    }
    
    // ----- fill out commands_win and info_win
    // info win
    (void) wattron(ui.info_win, A_UNDERLINE);
    (void) mvwprintw(ui.info_win, 0, 0, "Material Painting");
    (void) wattroff(ui.info_win, A_UNDERLINE);
    // commands_win
    (void) mvwprintw(ui.commands_win, 0, 0, "The cursor is the highlighted square on the left");
    (void) mvwprintw(ui.commands_win, 1, 0, "Movement Commands:");
    (void) mvwprintw(ui.commands_win, 2, 0, "Arrow keys: move cursor");
    (void) mvwprintw(ui.commands_win, 3, 0, "WASD: move grid around");

    (void) mvwprintw(ui.commands_win, 1, 30, "Commands:");
    (void) mvwprintw(ui.commands_win, 2, 30, "space: set square under cursor to selected material");
    (void) mvwprintw(ui.commands_win, 3, 30, "n/p: traverse up/down material options");
    (void) mvwprintw(ui.commands_win, 4, 30, "q: toggle wall at cursor");
    (void) mvwprintw(ui.commands_win, 5, 30, "F2: submit information");
    // ----- define synonyms that are used in the function
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
    WINDOW *v_seperator;
    ITEM **menu_items;
    MENU *materials_menu;
    int half_length = getmaxx(ui.main_win) / 2;
        //TODO try making this a macro
    int c = -1;
    int cur_index = 0;
    ITEM *cur_item;
    int return_val = 0;

    struct coord grid_end = {0, 0};

    GRID_EDITOR grid_editor;
    
    // grid win is on the left and menu_win is on the right

    // even: (grid|sep|menu) __|_|_
    // odd: (grid|sep|menu) _|_|_
    grid_win = derwin(ui.main_win, 0, half_length, 0, 0);
    
    v_seperator = derwin(ui.main_win, 0, 1, 0, half_length);

    menu_win = derwin(ui.main_win, 0, 0, 0, half_length + 1);
    sub_win = derwin(menu_win, 0, 0, 1, 0);

    //(void) box(ui.info_win, 0, 0);
    //(void) box(ui.main_win, 0, 0);
    //(void) box(ui.seperator, 0, 0);
    //(void) box(ui.commands_win, 0, 0);
    //(void) box(grid_win, 0, 0);
    //(void) box(v_seperator, 0, 0);
    //(void) box(menu_win, 0, 0);
    //(void) box(sub_win, 0, 0);

    //wnoutrefresh(stdscr);
    //wnoutrefresh(ui.info_win);
    //wnoutrefresh(ui.main_win);
    //wnoutrefresh(ui.seperator);
    //wnoutrefresh(ui.commands_win);
    //wnoutrefresh(grid_win);
    //wnoutrefresh(v_seperator);
    //wnoutrefresh(menu_win);
    //wnoutrefresh(sub_win);
    //doupdate();
    //getch();

    // ----- render a dividing line on the seperator window
    for (int i = 0; i < LINES; i++) {
        (void) mvwprintw(v_seperator, i, 0, "|");
    }

    // ----- print menu
    (void) mvwprintw(menu_win, 0, 0, "Material Options");

    // ----- print the squares
    getmaxyx(grid_win, grid_end.y, grid_end.x);
    (void) init_grid_editor(&grid_editor, target_grid
        , grid_end, grid_win, true);

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
        update_ui(&ui);
        (void) wnoutrefresh(grid_win);
        (void) wnoutrefresh(v_seperator);
        (void) wnoutrefresh(menu_win);
        (void) doupdate();

        // --- process input
        c = wgetch(ui.main_win);
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
    (void) delwin(v_seperator);

    // ----- free ui windows
    del_ui(&ui);

    return return_val;
}
