#include "main.h"


static char *start_menu_options[] = {
    "Start Campaign",
    "Load Campaign",
    "Exit"
};

//static func_pointer start_menu_fp[] = {
//    {}
//}


int start_menu()
    // contained start menu for the main function
    // returns -1 if the menu fails to be created
{
    // ----- init
    ITEM **start_menu_items;
    MENU *start_menu;
    int n_choices = 0;
    int c = 0;
    int return_val = 0;
    /*@unused@*/ ITEM *cur_item = NULL;
    /*@unused@*/ int menu_index = -1;

    n_choices = ARRAY_LEN(start_menu_options);
    start_menu_items = calloc((size_t) n_choices + 1
        , sizeof(*start_menu_items));

    // check if the calloc succeeded
    if (start_menu_items == NULL) {
        (void) mvprintw(0, 0, "error when allocating"
            " data to start_menu_items, please fix.");
        (void) getch();
        (void) endwin();
        exit(EXIT_FAILURE);
    }

    // set the items 
    for (int i = 0; i < (int) n_choices; i++) {
        start_menu_items[i] = new_item(start_menu_options[i]
            , start_menu_options[i]);
    }

    // menu requires this to work
    start_menu_items[n_choices] = (ITEM *) NULL;

    // create menu
    start_menu = new_menu((ITEM **) start_menu_items);
    if (start_menu == NULL) {
        return_val = -1;
        goto null_menu_exit;
    }

    (void) post_menu(start_menu);
    (void) refresh();
    do {
        c = getch();
        switch (c) {
            case KEY_DOWN:
                (void) menu_driver(start_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                (void) menu_driver(start_menu, REQ_UP_ITEM);
                break;
            case 10: // enter
                //cur_item = current_item(start_menu);
                //menu_index = item_index(cur_item);
                break;
        }
    } while (c != 10);

    // TODO use menu_index to select function

    // free values
    (void) unpost_menu(start_menu);
    (void) free_menu(start_menu);

null_menu_exit:
    for (int i = 0; i < (int) n_choices + 1; i++) {
        (void) free_item(start_menu_items[i]);
    }
    
    free(start_menu_items);

    return return_val;
}

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
    menu_items = calloc((size_t) mat_list_len + 1
        , sizeof(*menu_items));

    if (menu_items == NULL) {
        return_val = -1;
        goto null_menu_exit;
    }

    // set the items
    for (int i = 0; i < mat_list_len; i++) {
        menu_items[i] = new_item(mat_list[i].name
            , mat_list[i].desc);
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

int creature_creation_menu(struct campaign *target_campaign)
    // TODO add another window for the grid editor, perhaps
    // a popup
    // TODO support adding multiple characters
{
    struct creature *creature_list = target_campaign->creature_list;
    GRID_EDITOR grid_editor;

    // ----- initialize info for get_multi_input
    char print_char_string[2]; // a string to store the print char
        // , which will then be converted into just a char
    (void) memset(print_char_string, 0, sizeof(print_char_string));

    // get_multi_input argument values
    int text_pos[3] = {1, 3, 5};
    // NOTE: accessing note.string like this shouldn't be done
    char *dest[3] 
        = {creature_list[0].name, print_char_string
            , creature_list[0].note.string};
    int max_lens[3] = {MAX_CHAR, 2, MAX_CHAR};

    (void) mvprintw(0, 0, "Name: ");
    (void) mvprintw(2, 0, "Char: ");
    (void) mvprintw(4, 0, "Notes: ");

    // ----- get information about the character
    (void) get_multi_input(dest, 3, max_lens, text_pos);

    // ----- convert the print_char_string into a char and save it into
        // the creature
    creature_list[0].print_char = print_char_string[0];

    // ----- place the character menu
    // turn off the cursor because it's interfering with the grid
    (void) curs_set(0);
    int c = 0;
    struct coord window_end = {LINES, COLS};
    (void) init_grid_editor(&grid_editor
        , &target_campaign->encounter_grid, window_end, stdscr);

    do {
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
            case ' ':
                (void) grid_editor_driver(&grid_editor
                    , &creature_list[0].name, PLACE_CHAR);
                goto end;
        }
    } while (c != KEY_F(2));

end:
    return 0;
}

// a calloc'd array of possibly null ITEM pointers
typedef /*@only@*/ pos_null_ITEM_ptr * calloc_null_ITEM_ptr;

// possibly null struct item pointer
typedef /*@null@*/ struct item * pos_null_item_ptr;

struct changable_menu {
    /*@temp@*/ WINDOW *window;
    /*@temp@*/ WINDOW *sub_window; // associated sub window
    /*@owned@*/ MENU *menu; // associated menu
    
    pos_null_ITEM_ptr empty_array[2]; // array that displays
        // information when the main arrays are null. its length is
        // always 2

    calloc_null_ITEM_ptr arrays[2]; // 2 main arrays that
        // store the ITEM pointers
        // note that this is an array of 2 heap arrays
    int array_len;
    int array_next_empty;
    int selected_array;
};

static void init_changable_menu(/*@out@*/ struct changable_menu *target
    , int array_len)
{
    // ----- memset the target
    target = memset(target, 0, sizeof(*target));
    if (target == NULL) {
        (void) endwin();
        log_err("can't memset");
        exit(EXIT_FAILURE);
    }

    // ----- define the empty_array
    target->empty_array[0] = new_item("<empty>", "");
    target->empty_array[1] = NULL;

    // ----- set the other values
    target->array_len = array_len;
    target->selected_array = 0; // technically redundant

    // ----- initialize the two item arrays
    for (int i = 0; i < 2; i++) {
        target->arrays[i] = NULL; // prevent unallocated data
            // from showing up

        // allocate the space for an array
            // +1 to ensure that the last value is null (which is
            // required for the menu)
        target->arrays[i] = calloc((size_t)array_len + 1
        , sizeof(*target->arrays));

        if (target->arrays[i] == NULL) {
            (void) endwin();
            log_err("can't allocate memory");
            exit(EXIT_FAILURE);
        }

        // clear the allocated array
        target->arrays[i] = memset(target->arrays[i]
            , 0, (array_len + 1)
            * sizeof(*target->arrays[i]));
    }
}

static void set_changable_menu(struct changable_menu *target
    , pos_null_item_ptr *source_array)
    // NOTE: sets target->array_next_empty
    // also stops at the first NULL
{
    // --- initialize items into the 0 position and store in the 
        // item array
    for (int i = 0; i < target->array_len; i++) {
        if (source_array[i] == NULL) {
            target->arrays[0][i] = NULL;
            target->array_next_empty = i; // set array_next_empty
                // to its proper value
            // TODO maybe check if the source array properly
                // ends at this position

            break;
        } else if (source_array[i]->name[0] == '\0') {
            // debug
            target->arrays[0][i] = new_item("<no name>", "");
        } else {
            target->arrays[0][i]
                = new_item(source_array[i]->name, "");
        }
        // check if an item is created
        if (target->arrays[0][i] == NULL) {
            (void) endwin();
            log_err("can't allocate item");
            exit(EXIT_FAILURE);
        }
    }

    if (target->menu != NULL) {
        (void) endwin();
        log_err("menu already defined");
        exit(EXIT_FAILURE);
    }
    if (target->array_next_empty > 0) {
        target->menu = new_menu((ITEM **) target->arrays[0]);
    } else {
        target->menu = new_menu((ITEM **) target->empty_array);
    }
    
    if (target->menu == NULL) {
        (void) endwin();
        log_err("can't create menu");
        exit(EXIT_FAILURE);
    }
}

static void ncurses_error(WINDOW *win, int y, int x, const char *name, int return_val)
{
    char *error_str = "";
    switch (return_val) {
        case E_OK:
            error_str = "E_OK";
            break;
        case E_BAD_ARGUMENT:
            error_str = "E_BAD_ARGUMENT";
            break;
        case E_NOT_CONNECTED:
            error_str = "E_NOT_CONNECTED";
            break;
        case E_POSTED:
            error_str = "E_POSTED";
            break;
        case E_SYSTEM_ERROR:
            error_str = "E_SYSTEM_ERROR";
            break;
        case E_CONNECTED:
            error_str = "E_CONNECTED";
            break;
        default:
            error_str = "unknown error";
    }
    (void) mvwprintw(win, y, x, "\"%s\" returned %s (%d)", name, error_str, return_val);
}

static void transfer_item(struct changable_menu *source,
    struct changable_menu *dest, ITEM *target_item)
{
    // debug
    (void) wmove(source->window, LINES - 1, 0);
    (void) wclrtoeol(source->window);
    (void) wmove(dest->window, LINES - 1, 0);
    (void) wclrtoeol(dest->window);

    // skip if the source array is empty
    if (source->array_next_empty == 0) {
        return;
    }
    // skip when the destination is full
    if (dest->array_next_empty == dest->array_len) {
        (void) mvwprintw(dest->window, LINES - 1, 0
            , "is full");
        return;
    }

    // SPLINT_NOTE: target_item is technically
        // dependent, but the storage exists
        // here so there shouldn't be any issues
        // but splint thinks there is one.
        // my temp solution is to add temp to current_item

    int target_item_index = item_index(target_item);
    int return_1 = 0;

    // add the selected item to the inventory
    dest->arrays[dest->selected_array]
        [dest->array_next_empty] = target_item;

    dest->array_next_empty++;

    // clear the opposite array of sel
    source->arrays[1 - source->selected_array] = memset(
        source->arrays[1 - source->selected_array], 0
        , sizeof(*source->arrays) * source->array_len);

    // copy the selected array to its opposite array
    // but skip the selected item
    // TODO may need a check if target_item_index
        // is within bounds
    int dest_index = 0;
    for (int i = 0; i < source->array_len; i++) {
        if (i != target_item_index) {
            source->arrays[1 - source->selected_array][dest_index++]
                = source->arrays[source->selected_array][i];
        }
    }
    source->array_next_empty--;
    
    (void) unpost_menu(source->menu);
    (void) unpost_menu(dest->menu);

    // set the new selected_sel_array
    // TODO write a display function to display
        // all of the empty positions
    source->selected_array = 1 - source->selected_array;

    // update the menus
    // display the empty array if the item array is empty
    if (source->array_next_empty == 0) {
        return_1 = set_menu_items(source->menu
            , (ITEM **) source->empty_array);
    } else {
        return_1 = set_menu_items(source->menu
            , (ITEM **) source->arrays[source->selected_array]);
    }
    ncurses_error(source->window, LINES - 1, 0
        , "set_menu_items(sel_menu)"
        , return_1);

    ncurses_error(dest->window, LINES - 1, 0
        , "set_menu_items(dest_menu)",
        set_menu_items(dest->menu
            , (ITEM **) dest->arrays[dest->selected_array]));

    // post
    ncurses_error(dest->window, LINES - 2, 0, "post(dest_menu)",
        post_menu(dest->menu));
    ncurses_error(source->window, LINES - 2, 0, "post(sel_menu)",
        post_menu(source->menu));
}

static void item_inventory_menu(struct campaign *target_campaign
    , struct item *target_item)
    // displays two menus, the item selection menu (sel)
    // and the inventory menu (inv)
    // it allows moving items back and forth between
    // the arrays
    // TODO save the information into the campaign correctly
        // use item_userptr to do this
{
    // ----- init the changable menus
    struct changable_menu sel; // selection menu (list of all items)
    struct changable_menu inv; // inventory menu (list of items in
        // inventory)
    init_changable_menu(&sel, target_campaign->item_list_len);
    init_changable_menu(&inv, target_item->inventory_len);

    // ----- define the changable menu sel
    sel.array_next_empty = target_campaign->next_empty_item;
    sel.array_len = target_campaign->item_list_len;
    sel.selected_array = 0;

    pos_null_item_ptr sel_source_array[sel.array_len];
    //int selected_sel_array = 0;
    (void) memset(sel_source_array, 0, sizeof(sel_source_array));
    
    //int sel_array_next_empty = target_campaign->next_empty_item;
    // NOTE: length is the full length, but this may include NULL pointers
    // next_empty gives the length of the filled out portion

    // --- create an array of pointers for create_menu
        // from the campaign's item list
    for (int i = 0; i < sel.array_len; i++) {
        // check if an item is invalid
        if (target_campaign->item_list[i].name[0] != '\0') {
            sel_source_array[i] = &target_campaign->item_list[i];
        } else {
            sel_source_array[i] = NULL;
        }
    }

    set_changable_menu(&sel, sel_source_array);

    // ----- define the changable menu inv
    inv.array_len = target_item->inventory_len;
    inv.array_next_empty = target_item->num_inv_items;
    inv.selected_array = 0;

    set_changable_menu(&inv, target_item->inventory);

    // ---- windows
    WINDOW *seperator_win; // seperators
    int half_length = COLS / 2;

    seperator_win = newwin(0, 0, 0, 0);
    sel.window = newwin(0, half_length, 0, 0);
    sel.sub_window = derwin(sel.window, 0, 0, 1, 0);
    inv.window = newwin(0, half_length - 1, 0, half_length + 1);
    inv.sub_window = derwin(inv.window, 0, 0, 1, 0);

    // ----- post values (not ideal programming)
    (void) set_menu_win(sel.menu, sel.window);
    (void) set_menu_sub(sel.menu, sel.sub_window);
    (void) post_menu(sel.menu);

    (void) set_menu_win(inv.menu, inv.window);
    (void) set_menu_sub(inv.menu, inv.sub_window);
    (void) post_menu(inv.menu);

    // --- add seperator
    mvwvline(seperator_win, 0, half_length, ACS_VLINE, LINES);

    //(void) box(sel_win, 0, 0);
    //(void) box(sel_sub, 0, 0);
    //(void) box(inv_win, 0, 0);

    // ----- refresh screen
    (void) wnoutrefresh(stdscr);
    (void) wnoutrefresh(seperator_win);
    (void) wnoutrefresh(inv.window);
    (void) wnoutrefresh(inv.sub_window);
    (void) wnoutrefresh(sel.window);
    (void) wnoutrefresh(sel.sub_window);
    (void) doupdate();

    // ----- input loop
    int selected_menu_index = 0;
        // the index of menu_list that the cursor menu is on
        // sel_menu on left and inv_menu on right
    /*@dependent@*/ MENU *menu_list[2] = {sel.menu, inv.menu};
        // set up pointers for easy access of both menus
        // in the input logic
    /*@null@*/ ITEM *selected_item = NULL;
    /*@null@*/ const char *selected_item_name = NULL;
    int c = 0;
    struct changable_menu *source;
    struct changable_menu *dest;
        
    while (true) {
        c = getch();

        // exit when the exit key is pressed
        if (c == KEY_F(2)) {
            break;
        }

        switch (c) {
            // scroll up or down the menu
            case KEY_UP:
                (void) menu_driver(menu_list[selected_menu_index]
                    , REQ_UP_ITEM);
                break;
            case KEY_DOWN:
                (void) menu_driver(menu_list[selected_menu_index]
                    , REQ_DOWN_ITEM);
                break;
            // change to the left or right menu
            case KEY_LEFT:
            case KEY_RIGHT:
                // sets selected_menu_index to the other value
                    // (0 -> 1 and 1 -> 0)
                selected_menu_index = 1 - selected_menu_index;
                break;
            case 32: // space
                // could be made more clean but its fine
                // perhaps do some array stuff again
                if (selected_menu_index == 0) {
                    // move the selected one from sel to inv
                    source = &sel;
                    dest = &inv;
                } else {
                    // move the selected one from inv to sel
                    source = &inv;
                    dest = &sel;
                }

                selected_item = current_item(source->menu);
                if (selected_item == NULL) {
                    continue;
                } 
                selected_item_name = item_name(selected_item);

                if (selected_item_name == NULL) {
                    continue;
                }

                transfer_item(source, dest, selected_item);
                break;
        }
        (void) pos_menu_cursor(menu_list[selected_menu_index]);

        (void) wnoutrefresh(stdscr);
        (void) wnoutrefresh(seperator_win);
        // write to screen such that the cursor is in the right position
        if (selected_menu_index == 0) {
            (void) wnoutrefresh(inv.window);
            (void) wnoutrefresh(inv.sub_window);
            (void) wnoutrefresh(sel.window);
            (void) wnoutrefresh(sel.sub_window);
        } else {
            (void) wnoutrefresh(sel.window);
            (void) wnoutrefresh(sel.sub_window);
            (void) wnoutrefresh(inv.window);
            (void) wnoutrefresh(inv.sub_window);
        }
        (void) doupdate();
    }

    // ----- free menu itmes
    (void) unpost_menu(sel.menu);
    (void) free_menu(sel.menu);
    (void) unpost_menu(inv.menu);
    (void) free_menu(inv.menu);

exit:
    // ----- free items
    // NOTE: all the allocated items will exist between the selected
        // inv_array and the selected sel_array
        // since no items are created or destroyed when moving between
        // inv_array[0] to inv_array[1] (same with sel_array)
    for (int j = 0; j < inv.array_len; j++) {
        if (inv.arrays[inv.selected_array][j] != NULL)
            (void) free_item(inv.arrays[inv.selected_array][j]);
    }
    free(inv.arrays[0]);
    free(inv.arrays[1]);

    for (int j = 0; j < sel.array_len; j++) {
        if (sel.arrays[sel.selected_array][j] != NULL)
            (void) free_item(sel.arrays[sel.selected_array][j]);
    }
    free(sel.arrays[0]);
    free(sel.arrays[1]);

    // --- free the item in the empty arrays
    (void) free_item(inv.empty_array[0]);
    (void) free_item(sel.empty_array[0]);

    // ----- delete all windows
    (void) delwin(seperator_win);
    (void) delwin(sel.window);
    (void) delwin(inv.window);
    (void) delwin(sel.sub_window);
    (void) delwin(inv.sub_window);

    // ----- clear screen
    (void) erase();
    (void) refresh();
}

// TODO make item inventory stuff its own function (I will need it more than once)
int item_creation_menu(struct campaign *target_campaign)
    // initalizes and defines an item based on user input
{
    struct item *target_item;
    char print_char_string[2]; // a string to store the print char
        // , which will then be converted into just a char
    (void) memset(print_char_string, 0, sizeof(print_char_string));
    char *pointer_array[3];
    int text_pos[3];
    int max_lens[3];
    int c = 0;

    // ----- set up get_multi_input
    while (true) {
        target_item = &target_campaign->item_list
            [target_campaign->next_empty_item++];

        // NOTE: accessing note.string like this shouldn't be done
        pointer_array[0] = target_item->name;
        pointer_array[1] = print_char_string;
        pointer_array[2] = target_item->note.string;
        
        text_pos[0] = 1;
        text_pos[1] = 3;
        text_pos[2] = 5;

        max_lens[0] = MAX_CHAR;
        max_lens[1] = 2;
        max_lens[2] = MAX_CHAR;

        (void) mvprintw(0, 0, "Name: ");
        (void) mvprintw(2, 0, "Char: ");
        (void) mvprintw(4, 0, "Notes: ");

        (void) get_multi_input(pointer_array, 3, max_lens, text_pos);

        c = getch();

        if (c == KEY_F(2)) {
            item_inventory_menu(target_campaign, target_item);
            break;
        }
        (void) erase();
        // ----- deal with inventory
    }
    
    return 0;
}
