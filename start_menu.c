#include "main.h"

static char *start_menu_options[] = {
    "Start Campaign",
    "Load Campaign",
    "Exit"
};

//static func_pointer start_menu_fp[] = {
//    {}
//}

struct coord {
    int y;
    int x;
};

int start_menu()
    // contained start menu for the main function
{
    // ----- init
    ITEM **start_menu_items;
    MENU *start_menu;
    int n_choices = 0;
    int c = 0;
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

    for (int i = 0; i < (int) n_choices + 1; i++) {
        (void) free_item(start_menu_items[i]);
    }
    
    free(start_menu_items);

    return 0;
}

int start_campaign(/*@unused@*/ struct campaign *target_campaign)
    // campaign has to be initialized and defined
{
    char note_temp[MAX_CHAR];
    int text_pos[] = {1, 3};
    int text_pos_len = ARRAY_LEN(text_pos);

    (void) memset(note_temp, 0, sizeof(note_temp));
    char *save_ptrs[] = {target_campaign->name, note_temp};

    (void) clear(); // clear stdscr from the menu in start_menu
    (void) mvprintw(0, 0, "Name: ");
    (void) mvprintw(2, 0, "Notes: ");

    (void) mvprintw(LINES - 1, 0, "Press F1 to save values");
    (void) mvprintw(LINES - 2, 0, "Press F2 to move on");

    (void) get_multi_input(save_ptrs, text_pos_len
        , MAX_CHAR, text_pos);

    // TODO replace with note setter function when that is done
    strncpy(target_campaign->note.string, note_temp
        , (size_t) target_campaign->note.len);
    target_campaign->note.string[target_campaign->note.len - 1] = '\0';
    (void) getch();

    return 0;
}

static void print_grid(struct grid *target_grid, WINDOW *target_window)
    // prints the grid out into a target window
    // assumes the target window is larger than the squares
    // TODO allow moving around the grid
{
    for (int i = 0; i < target_grid->max_y; i++) {
        for (int j = 0; j < target_grid->max_x; j++) {
            (void) mvprintw_square(i, j, &target_grid->squares[i][j]
                , target_window);
        }
    }
}

int start_encounter(struct grid *target_grid
    , struct material *mat_list, int mat_list_len)
    // TODO: allow support for large grids (scrolling)
{
    if (mat_list_len <= 0) {
        return -1;
    }
    // ----- init
    WINDOW *grid_win;
    WINDOW *menu_win;
    WINDOW *sub_win; // sub_win is the sub window of menu_win
    ITEM **menu_items;
    MENU *materials_menu;
    int half_length = COLS / 2;
    int c = -1;
    // TODO make the case when target_grid is larger than the window
    struct coord cursor = {-1, -1};
    struct coord max_cursor = {-1, -1};
    struct coord old_cursor = {-1, -1};
    int cur_index = 0;
    ITEM *cur_item;
    struct square *cursor_square;
    
    // grid win is on the left and menu_win is on the right

    grid_win = newwin(LINES, half_length, 0, 0);
    menu_win = newwin(LINES, half_length, 0, half_length + 1);
    sub_win = derwin(menu_win, LINES - 1, half_length, 1, 0);
        // TODO not perfect but fix later (two half_length / 2 windows
            // may be one short if the length is odd)

    // ----- render a dividing line on the grid side (so the menu side is
        // left untouched for the menu
    for (int i = 0; i < LINES; i++) {
        (void) mvwprintw(grid_win, i, COLS / 2 - 1, "|");
    }

    // ----- print menu
    (void) mvwprintw(menu_win, 0, 0, "Material Options");

    // ----- print the squares
    print_grid(target_grid, grid_win);

    // ----- create menu for materials
    // define the item array
    menu_items = calloc((size_t) mat_list_len + 1
        , sizeof(*menu_items));

    if (menu_items == NULL) {
        return -1;
    }

    // set the items
    for (int i = 0; i < mat_list_len; i++) {
        menu_items[i] = new_item(mat_list[i].name
            , mat_list[i].desc);
    }
    
    // define the menu
    materials_menu = new_menu((ITEM **)menu_items);

    (void) set_menu_win(materials_menu, menu_win);
    (void) set_menu_sub(materials_menu, sub_win);
    (void) post_menu(materials_menu);

    // --- set cursor and cursor max values 
        // TODO make this accomodate large grids
        // where the limits of the screen are the limiting factor
    cursor.y = target_grid->max_y / 2;
    cursor.x = target_grid->max_x / 2;
    max_cursor.y = target_grid->max_y - 1; 
        // - 1 is to convert length to index
    max_cursor.x = target_grid->max_x - 1;

    old_cursor = cursor; // set the starting old_cursor
        // to the starting cursor as a default
        // (they are the same, so its caught when showing and only
        // highlights the cursor w/o unhighlighting the old cursor)

    // ----- loop for inputs
    do {
        // --- show cursor for grid
        mvwchgat(grid_win
            , cursor.y, cursor.x, 1, A_REVERSE, 0, NULL);

        // only replace the old position with normal
            // if the cursor was moved
        if (cursor.y != old_cursor.y || cursor.x != old_cursor.x) {
            mvwchgat(grid_win
                , old_cursor.y, old_cursor.x, 1, A_NORMAL, 0, NULL);
            old_cursor = cursor; // copy the new cursor into the old
        }

        // --- update screen in CORRECT ORDER (stdscr has to be at bottom)
        (void) wnoutrefresh(stdscr);
        (void) wnoutrefresh(grid_win);
        (void) wnoutrefresh(menu_win);
        (void) doupdate();

        // --- process input
        c = getch();
        switch (c) {
            case KEY_UP:
                if (cursor.y > 0) {
                    cursor.y--;
                }
                break;
            case KEY_DOWN:
                if (cursor.y < max_cursor.y) {
                    cursor.y++;
                }
                break;
            case KEY_LEFT:
                if (cursor.x > 0) {
                    cursor.x--;
                }
                break;
            case KEY_RIGHT:
                if (cursor.x < max_cursor.x) {
                    cursor.x++;
                }
                break;
            case ' ': // set square to material

                // get the current selected material
                cur_item = current_item(materials_menu);
                if (cur_item == NULL) {
                    continue;
                }
                cur_index = item_index(cur_item);

                // use the material in the menu to set the square's
                    // material
                cursor_square = &target_grid->squares[cursor.x][cursor.y];

                cursor_square->material = &mat_list[cur_index];
                
                // update the square
                mvprintw_square(cursor.y, cursor.x
                    , cursor_square, grid_win);
                break;
            
            case 'n': // next material
                (void) menu_driver(materials_menu, REQ_DOWN_ITEM);
                break;
            case 'p': // previous material
                (void) menu_driver(materials_menu, REQ_UP_ITEM);
                break;
            case 'w': // toggle wall
                cursor_square = &target_grid->squares[cursor.x][cursor.y];
                cursor_square->is_wall = !(cursor_square->is_wall);
                mvprintw_square(cursor.y, cursor.x
                    , cursor_square, grid_win);
                break;

            // TODO: meke set wall and set material thing

        }
    } while(c != KEY_F(2));
        
    
    // ----- free menu itmes
    (void) unpost_menu(materials_menu);
    (void) free_menu(materials_menu);
    for (int i = 0; i < mat_list_len; i++) {
        (void) free_item(menu_items[i]);
    }
    free(menu_items);
    return 0;
}

int creature_creation_menu(struct creature *creature_list
    , int creature_list_len)
{
    int text_pos[1] = {1};
    char *dest[1] = {creature_list[0].name};
    (void) mvprintw(0, 0, "Name: ");

    (void) get_multi_input(dest, 1, MAX_CHAR, text_pos);
    (void) getch();

    return 0;
}

int get_multi_input(char **dest, int num_dest, int max_buffer_len
    , int *text_pos)
// TODO make it support other windows
// TODO make it support left and right editing
    // dest is an array of pointers to the destinations to write to
    // num_dest is the length of dest 
        //(the number of destinations there are)
    // max_buffer_len is the maximum number of characters to write 
        // to the buffer
    // text_pos is the position of where to show the inputted text
        //(this should be an empty line). Its length is num_dest
{
    // ----- checks
    if (text_pos == NULL || dest == NULL || *dest == NULL) {
        return -1;
    } else if (num_dest <= 0 || max_buffer_len <= 0) {
        return -1;
    }

    // ----- inits
    int c = 0;
    char c_char = '\0';

    char buffers[num_dest][max_buffer_len]; // stores the texts 
        // inputted by the user
    int buffer_indices[num_dest]; // stores the next clear 
        //position to write a character for each buffer
    int num_text = num_dest; // stores the number of input fields (texts)
    int cur_buffer = 0; // points to the current buffer 
        // that is being edited

    int cur_x = 0;
    int cur_y = 0;

    // ----- zero out the arrays
    for (int i = 0; i < num_dest; i++) {
        (void) memset(buffers[i], i, sizeof(buffers[i]));
    }
    (void) memset(buffer_indices, 0, sizeof(buffer_indices));

    // ----- set up the cursor in its correct position 
        // and set cur_y and cur_x
    (void) move(text_pos[0], 0);
    cur_y = getcury(stdscr);
    cur_x = getcurx(stdscr);

    // ----- input loop
    while (c != KEY_F(2)) {
        c = getch();
        switch (c) {
            case 263: // delete
                if (cur_x > 0) {
                    // only delete something if it is 
                        // something the user typed
                    (void) move(cur_y, --cur_x);
                    (void) delch();
                }
                if (buffer_indices[cur_buffer] > 0) // only mark a
                        // letter as deletable if it is greater than 0
                    buffer_indices[cur_buffer]--; // delete a char 
                        // from temp like an hdd deletes data (by just
                        // signalling that it can be overwritten)
                break;
            
            case KEY_DOWN:
                if (cur_buffer < num_text) {
                    cur_buffer++; // change the current buffer to edit
                    
                    // update cur_y and cur_x to reflect the 
                        // new current buffer
                    cur_y = text_pos[cur_buffer];
                    cur_x = buffer_indices[cur_buffer];
                    (void) move(cur_y, cur_x);
                }
                break;
            case KEY_UP:
                if (cur_buffer > 0) {
                    cur_buffer--; // change the current buffer to edit

                    // update cur_y and cur_x to reflect 
                        // the new current buffer
                    cur_y = text_pos[cur_buffer];
                    cur_x = buffer_indices[cur_buffer];
                    (void) move(cur_y, cur_x);
                }
                break;

            default: // typing regular characters
                // does not add any characters 
                    // if the max_buffer_len is reached
                if (buffer_indices[cur_buffer] == max_buffer_len - 1) {
                    // if the index points to the last empty character
                        // (since max_buffer_len is the max index)
                    continue;
                }
                c_char = (char) c;

                // remove all characters that aren't allowed 
                    // (alphabetical only for now) TODO change
                if (c_char != ' ' 
                    && (c_char < 'a' || c_char > 'z') 
                    && (c_char < 'A' || c_char > 'Z'))
                    continue;
                (void) addch((chtype) c_char);
                buffers[cur_buffer][buffer_indices[cur_buffer]++]
                    = c_char;
                cur_x++;
        }
    }

    // ----- null terminate string and save it to dest
    for (int i = 0; i < num_text; i++) {
        buffers[i][buffer_indices[i]] = '\0'; // end the string 
            // with a \0, avoiding any 'deleted'
            // (but still present) data from messing things up and
            // ensuring that the string is null terminated

        // copy to dest (safely)
        (void) strncpy(dest[i], buffers[i], (size_t)max_buffer_len);
        dest[i][max_buffer_len] = '\0';
    }
    return 0;
}
