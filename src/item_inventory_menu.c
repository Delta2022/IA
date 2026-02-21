#include "../include/main.h"

// a calloc'd array of possibly null ITEM pointers
typedef /*@only@*/ pos_null_ITEM_ptr * calloc_null_ITEM_ptr;

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
        (void) set_item_userptr(target->arrays[0][i], source_array[i]);
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

struct seperated_inventory {
    /*@temp@*/ pos_null_item_ptr *inventory;
    /*@temp@*/ int *inventory_len;
    /*@temp@*/ int *num_inv_items;
};

void inventory_menu(struct campaign *target_campaign
    , /*@null@*/ struct item *target_item
    , /*@null@*/ struct creature *target_creature)
    // displays two menus, the item selection menu (sel)
    // and the inventory menu (inv)
    // it allows moving items back and forth between the arrays
    // target_item and target_creature are mutually exlcusive. One
        // has to be NULL
        // or else it returns

    // TODO perhaps remove the ability for source to have its item
        // removed
    // or manage multiple items in the item creation menu
        // (aka. items can be duplicated when creating items, but
        // they can't be duplicated here)
    // NOTE: this only saves for the item inventory
        // i may need better checks to ensure items are unique
    // essentially xoring target_item and target_creature
{
    if (target_item == NULL && target_creature == NULL) {
        return;
    } else if (target_item != NULL && target_creature != NULL) {
        return;
    }
    // ----- create a seperated_inventory from either target_item
        // or target_creature
    struct seperated_inventory target_inventory;

    if (target_item != NULL) {
        target_inventory.inventory = target_item->inventory;
        target_inventory.inventory_len = &target_item->inventory_len;
        target_inventory.num_inv_items = &target_item->num_inv_items;
    } else if (target_creature != NULL) {
        target_inventory.inventory = target_creature->inventory;
        target_inventory.inventory_len = &target_creature->inventory_len;
        target_inventory.num_inv_items = &target_creature->num_inv_items;
    } else {
        return;
    }

    // ----- init the changable menus
    struct changable_menu sel; // selection menu (list of all items)
    struct changable_menu inv; // inventory menu (list of items in
        // inventory)
    init_changable_menu(&sel, target_campaign->item_list_len);
    init_changable_menu(&inv, *target_inventory.inventory_len);

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
    inv.array_len = *target_inventory.inventory_len;
    inv.array_next_empty = *target_inventory.num_inv_items;
    inv.selected_array = 0;

    set_changable_menu(&inv, target_inventory.inventory);

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

    // ----- transfer the inventory into the correct position
    /*@null@*/ struct item *storage_ptr = NULL;
    for (int i = 0; i < inv.array_len; i++) {
        if (inv.arrays[inv.selected_array][i] == NULL) {
            continue;
        }
        storage_ptr = item_userptr(inv.arrays[inv.selected_array][i]);

        if (storage_ptr == NULL) {
            continue;
        }
        target_inventory.inventory[i] = storage_ptr;
    }
    // ----- set the inventory's num_inv_items
        // array_next_empty and the number of items
        // is the same number
    *target_inventory.num_inv_items = inv.array_next_empty;

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
