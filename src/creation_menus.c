#include "../include/main.h"

#define MAIN_WIN_SPACE LINES * 3/4

int creature_creation_menu(struct campaign *target_campaign
    , /*@null@*/ struct creature *target_creature, bool is_skip_placement)
    // TODO add another window for the grid editor, perhaps
    // a popup
    // TODO rename to creature_edit_menu or something
    // prompts the user for information regarding the creature
    // pointed to by target_creature
    // if target_creature is null, a new creature is created in the
    // campaign
    // the user will not be prompted to place a creature
    // if skip_placement is true
    // return of -1 means that the menu couldn't create a ui_windows
{
    // ----- initialize windows
    struct ui_windows ui;
    if (init_ui(&ui, 0) == -1) {
        return -1;
    }

    // ----- write information to info_win and commands_win
    // info_win
    (void) wattron(ui.info_win, A_UNDERLINE);
    (void) mvwprintw(ui.info_win, 0, 0, "Create Creature");
    (void) wattroff(ui.info_win, A_UNDERLINE);

    // commands_win
    (void) mvwprintw(ui.commands_win, 0, 0, "Commands");
    (void) mvwprintw(ui.commands_win, 1, 0, "Up arrow/down arrow:"
        " traverse through input fields");
    (void) mvwprintw(ui.commands_win, 2, 0, "F2: submit information");
    (void) mvwprintw(ui.commands_win, 3, 0, "Note: only characters"
        " a-z and A-Z are allowed.");

    //struct creature *creature_list = target_campaign->creature_list;
    if (target_creature == NULL) {
        target_creature = &target_campaign->creature_list
                [target_campaign->next_empty_creature++];
    }
    GRID_EDITOR grid_editor;

    // ----- initialize info for get_multi_input
    char print_char_string[2]; // a string to store the print char
        // , which will then be converted into just a char
    (void) memset(print_char_string, 0, sizeof(print_char_string));

    // put in the print_char of the creature in case we are editing
    print_char_string[0] = target_creature->print_char;

    // get_multi_input argument values
    int text_pos[3] = {1, 3, 5};
    // NOTE: accessing note.string like this shouldn't be done
    char *dest[3] 
        = {target_creature->name, print_char_string
            , target_creature->note.string};
    int max_lens[3] = {MAX_CHAR, 2, MAX_CHAR};

    (void) mvwprintw(ui.main_win, 0, 0, "Name: ");
    (void) mvwprintw(ui.main_win, 2, 0, "Char: ");
    (void) mvwprintw(ui.main_win, 4, 0, "Notes: ");

    update_ui(&ui);
    (void) doupdate();
    // ----- get information about the character
    (void) get_multi_input(ui.main_win, dest, 3, max_lens, text_pos);

    // ----- convert the print_char_string into a char and save it into
        // the creature
    target_creature->print_char = print_char_string[0];

    // ----- delete windows for inventory_menu
    del_ui(&ui);
    // ----- set creature inventory
    inventory_menu(target_campaign, NULL, target_creature);

    // ----- initialize windows again
    if (init_ui(&ui, 0) == -1) {
        return -1;
    }

    // ----- skip the placement if is_skip_placement is true
    if (is_skip_placement) {
        goto end;
    }
    // ----- write information to info_win and commands_win
    // info_win
    (void) wattron(ui.info_win, A_UNDERLINE);
    (void) mvwprintw(ui.info_win, 0, 0, "Place Creature on Grid");
    (void) wattroff(ui.info_win, A_UNDERLINE);

    // commands_win
    (void) mvwprintw(ui.commands_win, 0, 0, "Commands");
    (void) mvwprintw(ui.commands_win, 1, 0, "Arrow keys: move cursor");
    (void) mvwprintw(ui.commands_win, 2, 0, "WASD: move grid around");

    (void) mvwprintw(ui.commands_win, 3, 0, "space: place creature"
        " at cursor");
    (void) mvwprintw(ui.commands_win, 4, 0, "F2: place creature"
        " at cursor");
    update_ui(&ui);
    // ----- place the character on the grid
    // turn off the cursor because it's interfering with the grid
    (void) curs_set(0);
    int c = 0;
    struct coord window_end = {LINES, COLS};
    (void) init_grid_editor(&grid_editor
        , &target_campaign->encounter_grid, window_end, ui.main_win
            , false);

    while (true) {
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
                    , &target_creature->name, PLACE_CHAR);
                goto end;
        }
    }

end:
    del_ui(&ui);
    return 0;
}

int item_creation_menu(struct campaign *target_campaign
    , /*@null@*/ struct item *target_item)
    // initalizes and defines an item based on user input
    // if target_item is NULL, a new item is created
    // if it isn't NULL, then target_item is edited
    // NOTE: only theoretically working
{
    // ----- init ui
    struct ui_windows ui;
    if (init_ui(&ui, 0) == -1) {
        return -1;
    }

    // ----- write values to ui.info_win and ui.commands_win
    // info win
    (void) wattron(ui.info_win, A_UNDERLINE);
    (void) mvwprintw(ui.info_win, 0, 0, "Create Item");
    (void) wattroff(ui.info_win, A_UNDERLINE);

    // commands
    (void) mvwprintw(ui.commands_win, 0, 0, "Commands");
    (void) mvwprintw(ui.commands_win, 1, 0, "Up arrow/down arrow:"
        " traverse through input fields");
    (void) mvwprintw(ui.commands_win, 2, 0, "F2: submit information");
    (void) mvwprintw(ui.commands_win, 3, 0, "Note: only characters"
        " a-z and A-Z are allowed.");
    update_ui(&ui);
    // -----
    char print_char_string[2]; // a string to store the print char
        // , which will then be converted into just a char
    (void) memset(print_char_string, 0, sizeof(print_char_string));
    char *pointer_array[3];
    int text_pos[3];
    int max_lens[3];


    // ----- set up get_multi_input
    if (target_item == NULL)
        target_item = &target_campaign->item_list
            [target_campaign->next_empty_item++];

    // ----- copy the print_char of the item to print_char_string in case
        // the item is being edited
    print_char_string[0] = target_item->print_char;

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

    (void) mvwprintw(ui.main_win, 0, 0, "Name: ");
    (void) mvwprintw(ui.main_win, 2, 0, "Char: ");
    (void) mvwprintw(ui.main_win, 4, 0, "Notes: ");

    (void) get_multi_input(ui.main_win, pointer_array, 3, max_lens
        , text_pos);

    // save print_char
    target_item->print_char = print_char_string[0];
    // ----- delete ui for inventory_menu
    del_ui(&ui);
    // ----- deal with inventory
    inventory_menu(target_campaign, target_item, NULL);
    
    return 0;
}

void square_note_creation_menu(struct campaign *target_campaign
    , struct coord position)
    // theoretically works
    // creates or edits the note at the coord position
{
    // ----- initialize windows
    struct ui_windows ui;
    if (init_ui(&ui, 0) == -1) {
        return;
    }
    
    // info win
    (void) wattron(ui.info_win, A_UNDERLINE);
    (void) mvwprintw(ui.info_win, 0, 0, "Positional Note Creation");
    (void) wattroff(ui.info_win, A_UNDERLINE);

    // commands
    (void) mvwprintw(ui.commands_win, 0, 0, "Commands");
    (void) mvwprintw(ui.commands_win, 1, 0, "F2: submit information");
    (void) mvwprintw(ui.commands_win, 2, 0, "Note: only characters"
        " a-z and A-Z are allowed.");
    update_ui(&ui);
    // -----
    
    struct square *target_square = &target_campaign->encounter_grid
        .squares[position.y][position.x];

    char *pointer_array[1] = {target_square->note.string};
    int text_pos[1] = {1};
    int max_lens[1] = {target_square->note.len};

    (void) mvwprintw(ui.main_win, 0, 0, "Notes");
    (void) get_multi_input(ui.main_win, pointer_array, 1, max_lens
        , text_pos);

    del_ui(&ui);
    (void) erase();
}

void material_creation_menu(struct campaign *target_campaign
    , /*@null@*/ struct material *target_material)
    // if target_material is NULL, then new materials are created
    // (in a loop)
    // otherwise the target_material is edited (once then exit)
{
    // ----- initialize windows
    struct ui_windows ui;
    if (init_ui(&ui, 0) == -1) {
        return;
    }

    // ----- write values to info_win
    // info_win
    (void) wattron(ui.info_win, A_UNDERLINE);
    (void) mvwprintw(ui.info_win, 0, 0, "Create Materials");
    (void) wattroff(ui.info_win, A_UNDERLINE);


    // -----
    bool is_loop;
    if (target_material == NULL) is_loop = true;
    else is_loop = false;
    int c = 0;

    while (true) {

        // check if there is space
        if (target_campaign->next_empty_material
            == target_campaign->material_list_len) {
            break;
        }

        // ----- write commands to commands_win
        (void) mvwprintw(ui.commands_win, 0, 0, "Commands");
        (void) mvwprintw(ui.commands_win, 1, 0, "Up arrow/down arrow:"
            " traverse through input fields");
        (void) mvwprintw(ui.commands_win, 2, 0, "F2: submit information");
        (void) mvwprintw(ui.commands_win, 3, 0, "Note: only characters"
            " a-z and A-Z are allowed.");

        update_ui(&ui);
        (void) doupdate();

        // ----- get user input for text
        if (target_material == NULL || is_loop)
            target_material = &target_campaign->material_list
                [target_campaign->next_empty_material++];

        char print_char_string[2];
        (void) memset(print_char_string, 0, sizeof(*print_char_string));

        // copy the material's print_char into print_char_string for
            // editing
        print_char_string[0] = target_material->print_char;

        // set values
        char *pointer_array[4] = {target_material->name
            , target_material->desc, print_char_string
            , target_material->note.string};
        int max_lens[4] = {target_material->name_len
            , target_material->desc_len, 2, target_material->note.len};
        int text_pos[4] = {1, 3, 5, 7};

        (void) mvwprintw(ui.main_win, 0, 0, "Name");
        (void) mvwprintw(ui.main_win, 2, 0, "Description");
        (void) mvwprintw(ui.main_win, 4, 0, "Print character");
        (void) mvwprintw(ui.main_win, 6, 0, "Notes");

        (void) get_multi_input(ui.main_win, pointer_array, 4, max_lens
            , text_pos);

        // move convert print_char_string from a string into a character
        target_material->print_char = print_char_string[0];

        // ----- exit and do not promp the user if is_loop is false
        if (!is_loop) goto exit;
        // ----- get user input for creating more materials
        // update commands
        (void) werase(ui.commands_win);
        (void) mvwprintw(ui.commands_win, 0, 0, "Commands");
        (void) wattron(ui.commands_win, A_REVERSE);
        (void) mvwprintw(ui.commands_win, 1, 0, "Keep creating"
            " materials?");
        (void) wattroff(ui.commands_win, A_REVERSE);
        (void) mvwprintw(ui.commands_win, 2, 0, "F2: continue");
        (void) mvwprintw(ui.commands_win, 3, 0, "any other character:"
            " quit");

        update_ui(&ui);
        (void) doupdate();
        // prompt the user
        c = wgetch(ui.main_win);

        if (c != KEY_F(2)) {
            break;
        }
        (void) werase(ui.main_win);
    }

exit:
    del_ui(&ui);
}
