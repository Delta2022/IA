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

static void init_item_array(/*@out@*/ calloc_null_ITEM_ptr item_array[]
    , /*@out@*/ MENU **dest_menu, pos_null_item_ptr *source_array
    , int source_array_len)
    // item_array must be array of length 2
    // dest_menu is a pointer to pointer to the menu because
    // dest_menu has to be pass by reference (aka the menu initialized
    // here moves onto the bigger scope) but new_menu
    // only returns pointer to menu, so I have to add another
    // pointer
{
    *dest_menu = NULL;
    
    // ----- initialize item arrays
    for (int i = 0; i < 2; i++) {
        item_array[i] = NULL; // prevent unallocated data
            // from showing up

        // +1 to ensure that the last value can be null (which is
        // required for the menu)
        item_array[i] = calloc((size_t)source_array_len + 1
        , sizeof(*item_array));

        if (item_array[i] == NULL) {
            // TODO expand this
            (void) endwin();
            exit(EXIT_FAILURE);
        }

        item_array[i] = memset(item_array[i], 0, (source_array_len + 1)
            * sizeof(*item_array[i]));
    }

    // --- initialize items into the 0 position and store in the 
        // item array
    for (int i = 0; i < source_array_len; i++) {
        if (source_array[i] == NULL) {
            item_array[0][i] = NULL;
            continue;
        } else if (source_array[i]->name[0] == '\0') {
            // debug
            item_array[0][i] = new_item("<no name>", "");
        } else {
            item_array[0][i] = new_item(source_array[i]->name, "");
        }
        // check if an item is created
        if (item_array[0][i] == NULL) {
            (void) endwin();
            exit(EXIT_FAILURE);
        }
    }


    // --- create menu
    // *dest_menu = new_menu((ITEM **) item_array[0]);

    //if (dest_menu == NULL) {
    //    (void) endwin();
    //    exit(EXIT_FAILURE);
    //}
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

// TODO make item inventory stuff its own function (I will need it more than once)
int item_creation_menu(struct campaign *target_campaign)
// initializes values for items
// generally this isn't named very well, but inv = item inventory menu
    // and sel = item selection menu
// returns
// 0: normal
// -1: error when allocating for item array
// -2: error when allocating items
// -3: error when allocating menu
{
    int return_val = 0;
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
            break;
        }
        (void) erase();
    }

    // TODO convert print_char_string properly

    // ----- create menus
    // TODO create a function that makes this cleaner
    WINDOW *seperator_win; // seperators
    WINDOW *sel_win; // item list to add to window
    WINDOW *sel_sub; 
    WINDOW *inv_win; // item list for inventory
    WINDOW *inv_sub; // item list for inventory
    int half_length = COLS / 2;

    seperator_win = newwin(0, 0, 0, 0);
    sel_win = newwin(0, half_length, 0, 0);
    sel_sub = derwin(sel_win, 0, 0, 1, 0);
    inv_win = newwin(0, half_length - 1, 0, half_length + 1);
    inv_sub = derwin(inv_win, 0, 0, 1, 0);

    // --- add seperator
    mvwvline(seperator_win, 0, half_length, ACS_VLINE, LINES);

    // ----- create items for when the item arrays are empty
    // TODO generalize this later
    // NOTE: different arrays are used to prevent issues with
        // connection
    pos_null_ITEM_ptr inv_empty_array[2] = {NULL, NULL};
    pos_null_ITEM_ptr sel_empty_array[2] = {NULL, NULL};
    
    inv_empty_array[0] = new_item("<empty>", "");
    sel_empty_array[0] = new_item("<empty>", "");
    
    // ----- create item selection menu
    calloc_null_ITEM_ptr sel_array[2];
    int sel_array_len = target_campaign->item_list_len;
    pos_null_item_ptr sel_source_array[sel_array_len];
    int selected_sel_array = 0;

    (void) memset(sel_source_array, 0, sizeof(sel_source_array));
    
    int sel_array_next_empty = target_campaign->next_empty_item;
    // NOTE: length is the full length, but this may include NULL pointers
    // next_empty gives the length of the filled out portion

    MENU *sel_menu = NULL;

    // --- create an array of pointers for create_menu
        // from the campaign's item list

    for (int i = 0; i < sel_array_len; i++) {
        // check if an item is invalid
        if (target_campaign->item_list[i].name[0] != '\0') {
            sel_source_array[i] = &target_campaign->item_list[i];
        } else {
            sel_source_array[i] = NULL;
        }
    }

    init_item_array(sel_array, &sel_menu, sel_source_array
        , sel_array_len);

    if (sel_array_next_empty > 0) {
        sel_menu = new_menu((ITEM **) sel_array[0]);
    } else {
        sel_menu = new_menu((ITEM **) sel_empty_array);
    }
    
    if (sel_menu == NULL) {
        (void) endwin();
        exit(EXIT_FAILURE);
    }

    (void) set_menu_win(sel_menu, sel_win);
    (void) set_menu_sub(sel_menu, sel_sub);
    (void) post_menu(sel_menu);

    // ----- create item inventory menu
    calloc_null_ITEM_ptr inv_array[2];
    int inv_array_len = target_item->inventory_len;
    int inv_array_next_empty = target_item->num_inv_items;
    int selected_inv_array = 0;

    MENU *inv_menu = NULL;

    init_item_array(inv_array, &inv_menu, target_item->inventory
        , target_item->inventory_len);

    if (inv_array_next_empty > 0 ) {
        inv_menu = new_menu((ITEM **) inv_array[0]);
    } else {
        inv_menu = new_menu((ITEM **) inv_empty_array);
    }

    if (inv_menu == NULL) {
        (void) endwin();
        exit(EXIT_FAILURE);
    }
    (void) set_menu_win(inv_menu, inv_win);
    (void) set_menu_sub(inv_menu, inv_sub);
    (void) post_menu(inv_menu);

    //(void) box(sel_win, 0, 0);
    //(void) box(sel_sub, 0, 0);
    //(void) box(inv_win, 0, 0);

    // ----- refresh screen
    (void) wnoutrefresh(stdscr);
    (void) wnoutrefresh(seperator_win);
    (void) wnoutrefresh(inv_win);
    (void) wnoutrefresh(inv_sub);
    (void) wnoutrefresh(sel_win);
    (void) wnoutrefresh(sel_sub);
    (void) doupdate();

    // ----- input loop
    int selected_menu_index = 0;
        // the index of menu_list that the cursor menu is on
        // sel_menu on left and inv_menu on right
    MENU *menu_list[2] = {sel_menu, inv_menu};
        // set up pointers for easy access of both menus
        // in the input logic
    /*@null@*/ ITEM *selected_item = NULL;
    int selected_item_index = 0;
    int dest_index = 0;
    /*@null@*/ const char *selected_item_name = NULL;
    /*@null@*/ ITEM *temp_storage = NULL; // stores the
        // null item whose place is taken when
        // an item is selected
    int return_1 = 0; // debug
    int return_2 = 0;
        
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

                    // debug
                    (void) wmove(sel_win, LINES - 1, 0);
                    (void) wclrtoeol(sel_win);
                    (void) wmove(inv_win, LINES - 1, 0);
                    (void) wclrtoeol(inv_win);

                    // skip when the destination is full
                    if (inv_array_next_empty == inv_array_len) {
                        (void) mvwprintw(inv_win, LINES - 1, 0
                            , "is full");
                        continue;
                    }

                    // skip if the source array is empty
                    if (sel_array_next_empty == 0) {
                        continue;
                    }

                    selected_item = current_item(sel_menu);
                    if (selected_item == NULL) {
                        continue;
                    } 
                    selected_item_name = item_name(selected_item);

                    if (selected_item_name == NULL) {
                        continue;
                    }

                    selected_item_index = item_index(selected_item);

                    // SPLINT_NOTE: selected_item is technically
                        // dependent, but the storage exists
                        // here so there shouldn't be any issues
                        // but splint thinks there is one.
                        // my temp solution is to add temp to current_item

                    // add the selected item to the inventory
                    inv_array[selected_inv_array]
                        [inv_array_next_empty++] = selected_item;

                    // clear the opposite array of sel
                    sel_array[1 - selected_sel_array] = memset(
                        sel_array[1 - selected_sel_array], 0
                        , sizeof(*sel_array) * sel_array_len);

                    // copy the selected array to its opposite array
                    // but skip the selected item
                    // TODO may need a check if selected_item_index
                        // is within bounds
                    dest_index = 0;
                    for (int i = 0; i < sel_array_len; i++) {
                        if (i != selected_item_index) {
                            sel_array[1 - selected_sel_array]
                                [dest_index++]
                                = sel_array[selected_sel_array]
                                    [i];
                        }
                    }
                    sel_array_next_empty--;
                    
                    (void) unpost_menu(sel_menu);
                    (void) unpost_menu(inv_menu);

                    // set the new selected_sel_array
                    // TODO handle when the item array turns all null
                    // TODO write a display function to display all of the empty positions
                    selected_sel_array = 1 - selected_sel_array;

                    // update the menus
                    // display the empty array if the item array is empty
                    if (sel_array_next_empty == 0) {
                        return_1 = set_menu_items(sel_menu
                            , (ITEM **) sel_empty_array);
                    } else {
                        return_1 = set_menu_items(sel_menu
                            , (ITEM **) sel_array[selected_sel_array]);
                    }
                    ncurses_error(sel_win, LINES - 1, 0, "set_menu_items(sel_menu)"
                        , return_1);

                    ncurses_error(inv_win, LINES - 1, 0, "set_menu_items(inv_menu)",
                        set_menu_items(inv_menu, (ITEM **) inv_array[selected_inv_array]));

                    // post
                    ncurses_error(inv_win, LINES - 2, 0, "post(inv_menu)",
                        post_menu(inv_menu));
                    ncurses_error(sel_win, LINES - 2, 0, "post(sel_menu)",
                        post_menu(sel_menu));
                } else {
                    // move the selected one from inv to sel

                }
                break;
        }
        (void) pos_menu_cursor(menu_list[selected_menu_index]);
        (void) wnoutrefresh(stdscr);
        (void) wnoutrefresh(seperator_win);
        (void) wnoutrefresh(inv_win);
        (void) wnoutrefresh(inv_sub);
        (void) wnoutrefresh(sel_win);
        (void) wnoutrefresh(sel_sub);
        (void) doupdate();
    }

    // ----- free menu itmes
    (void) unpost_menu(sel_menu);
    (void) free_menu(sel_menu);
    (void) unpost_menu(inv_menu);
    (void) free_menu(inv_menu);

exit:
    // ----- free items
    // NOTE: all the allocated items will exist between the selected
        // inv_array and the selected sel_array
        // since no items are created or destroyed when moving between
        // inv_array[0] to inv_array[1] (same with sel_array)
    for (int j = 0; j < inv_array_len; j++) {
        if (inv_array[selected_inv_array][j] != NULL)
            (void) free_item(inv_array[selected_inv_array][j]);
    }
    free(inv_array[0]);
    free(inv_array[1]);

    for (int j = 0; j < sel_array_len; j++) {
        if (sel_array[selected_sel_array][j] != NULL)
            (void) free_item(sel_array[selected_sel_array][j]);
    }
    free(sel_array[0]);
    free(sel_array[1]);

    // --- free the item in the empty arrays
    (void) free_item(inv_empty_array[0]);
    (void) free_item(sel_empty_array[0]);

    // ----- delete all windows
    (void) delwin(seperator_win);
    (void) delwin(sel_win);
    (void) delwin(inv_win);
    (void) delwin(sel_sub);
    (void) delwin(inv_sub);

    // ----- clear screen
    (void) erase();
    (void) refresh();
    return return_val;
}
