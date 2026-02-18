#include "../include/main.h"

static char *start_menu_options[] = {
    "Start Campaign",
    "Load Campaign",
    "Exit"
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
            , start_menu_options[i]);
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
        }
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
    switch (function_index) {
        case START_CAMPAIGN:
            // ----- initialize the campaign
            (void) start_campaign(target_campaign);

            material_creation_menu(target_campaign);
            
            (void) start_encounter(target_campaign);
            
            // ----- main loop
            while (return_val != 0) {
                return_val = main_menu(target_campaign);

                switch (return_val) {
                    case 1:
                        (void) creature_creation_menu(target_campaign);
                        break;
                    case 2:
                        p_note_creation_menu(target_campaign
                            , target_campaign->encounter_grid.cursor);
                        break;
                    case 3:
                        (void) item_creation_menu(target_campaign);
                        break;
                }
            }
            break;
        case LOAD_CAMPAIGN:
            break;
        case EXIT:
            return -1;

        default:
            return -2;
    }
    return 0;
}
