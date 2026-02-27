#include "../include/main.h"
#include "../include/menu_outputs.h"

static char *start_menu_options[] = {
    "Start Campaign",
    "Load Campaign",
    "Exit"
};

static char *start_menu_desc[] = {
    "Create a new campaign",
    "Load a saved campaign",
    "Exit program"
};

//static func_pointer start_menu_fp[] = {
//    {}
//}

#define START_CAMPAIGN  0
#define LOAD_CAMPAIGN   1
#define EXIT            2

int start_menu()
    // contained start menu for the main function
    // returns -1 if the menu fails to be created
{
    // ----- init windows
    //WINDOW *ui.main_win = newwin(0, COLS * 3/4, 0, 0);
    //WINDOW *ui.seperator = newwin(0, 1, 0, COLS * 3/4);
    //WINDOW *ui.commands_win = newwin(0, COLS * 1/4 - 1, 0, COLS * 3/4 + 1);

    struct ui_windows ui;
    if (init_ui(&ui, 0) == -1) {
        return -1;
    }
    // ----- check dimensions of windows
    //(void) box(ui.info_win, 0, 0);
    //(void) box(ui.main_win, 0, 0);
    //(void) box(ui.seperator, 0, 0);
    //(void) box(ui.commands_win, 0, 0);

    //wnoutrefresh(stdscr);
    //wnoutrefresh(ui.info_win);
    //wnoutrefresh(ui.main_win);
    //wnoutrefresh(ui.seperator);
    //wnoutrefresh(ui.commands_win);
    //doupdate();
    //getch();


    // ----- write values to ui.seperator and ui.commands_win
    // info win
    (void) wattron(ui.info_win, A_UNDERLINE);
    (void) mvwprintw(ui.info_win, 0, 0, "Start Menu");
    (void) wattroff(ui.info_win, A_UNDERLINE);
    
    // commands
    (void) mvwprintw(ui.commands_win, 0, 0, "Commands:");
    (void) mvwprintw(ui.commands_win, 1, 0, "Up arrow/down arrow: traverse"
        " through options");
    (void) mvwprintw(ui.commands_win, 2, 0, "Enter: choose option");

    // ----- init
    ITEM **start_menu_items;
    MENU *start_menu;
    int n_choices = 0;
    int c = 0;
    /*@unused@*/ int menu_index = -1;

    n_choices = ARRAY_LEN(start_menu_options);
    start_menu_items = calloc((size_t) n_choices + 1
        , sizeof(*start_menu_items));

    // check if the calloc succeeded
    if (start_menu_items == NULL) {
        (void) endwin();
        log_err("error when allocating data to start_menu_items"
            ", please fix.");
        exit(EXIT_FAILURE);
    }

    // set the items 
    for (int i = 0; i < (int) n_choices; i++) {
        start_menu_items[i] = new_item(start_menu_options[i]
            , start_menu_desc[i]);
    }

    // menu requires this to work
    start_menu_items[n_choices] = (ITEM *) NULL;

    // create menu
    start_menu = new_menu((ITEM **) start_menu_items);
    if (start_menu == NULL) {
        (void) endwin();
        log_err("error when creating menu (is null)");
        exit(EXIT_FAILURE);
    }

    (void) set_menu_win(start_menu, ui.main_win);
    (void) set_menu_sub(start_menu, ui.main_win);

    (void) post_menu(start_menu);

    update_ui(&ui);
    (void) doupdate();

    do {
        c = wgetch(ui.main_win);

        switch (c) {
            case KEY_DOWN:
                (void) menu_driver(start_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                (void) menu_driver(start_menu, REQ_UP_ITEM);
                break;
        }
        update_ui(&ui);
        (void) doupdate();
    } while (c != 10);

    // TODO use menu_index to select function
    /*@null@*/ ITEM *cur_item = current_item(start_menu);
    if (cur_item == NULL) {
        (void) endwin();
        log_err("current_item is null, when it should be an item");
        exit(EXIT_FAILURE);
    }
    int current_item_index = item_index(cur_item);

    // free values
    (void) unpost_menu(start_menu);
    (void) free_menu(start_menu);

    for (int i = 0; i < (int) n_choices + 1; i++) {
        (void) free_item(start_menu_items[i]);
    }
    
    free(start_menu_items);

    del_ui(&ui);

    (void) erase();

    return current_item_index;
}

int run_main_menu_function(int function_index
    , struct campaign *target_campaign)
    // runs the correct function based on the return value
    // of main_menu()
    // defines *target_campaign with the campaign that will be used
    // for the rest of the program
    // ----- returns -----
    // 0: ran functions as normal
    // -1: exit
    // -2: function_index wasn't a normal value
    // -3: malloc problem
{
    int return_val = -1;
    struct coord cursor = {0, 0};
    // ----- load or create a new campaign
    switch (function_index) {
        case LOAD_CAMPAIGN:
            load_campaign(target_campaign);
            break;
        case START_CAMPAIGN:
            // ----- initialize the campaign
            (void) start_campaign(target_campaign);
            material_creation_menu(target_campaign, NULL);
            (void) start_encounter(target_campaign);
            break;
        case EXIT:
            return -1;

        default:
            return -2;
    }
    // ----- main loop
    while (return_val != 0) {
        return_val = main_menu(target_campaign);

        // numbers are sourced from the return values of main
            // definitions are in include/menu_outputs.h
        switch (return_val) {
            case CREATURE_CREATION:
                (void) creature_creation_menu(target_campaign, NULL
                    , false);
                break;
            case P_NOTE_CREATION:
                cursor = get_grid_cursor(&target_campaign->encounter_grid);
                square_note_creation_menu(target_campaign
                    , cursor);
                break;
            case ITEM_CREATION:
                (void) item_creation_menu(target_campaign, NULL);
                break;
            case MATERIAL_CREATION:
                material_creation_menu(target_campaign, NULL);
                (void) start_encounter(target_campaign);
                break;
            case REPAINT_MATERIALS:
                (void) start_encounter(target_campaign);
                break;
            case MOVE_CREATURE:
                (void) move_menu(target_campaign);
                break;
            case EDIT_MENU:
                (void) edit_menu(target_campaign);
        }
    }

    //save_campaign(target_campaign);
    return 0;
}
