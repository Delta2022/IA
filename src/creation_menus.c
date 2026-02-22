#include "../include/main.h"

#define MAIN_WIN_SPACE LINES * 3/4

int creature_creation_menu(struct campaign *target_campaign)
    // TODO add another window for the grid editor, perhaps
    // a popup
    // TODO support adding multiple characters
{
    //struct creature *creature_list = target_campaign->creature_list;
    struct creature *target_creature = &target_campaign->creature_list
            [target_campaign->next_empty_creature++];
    GRID_EDITOR grid_editor;

    // ----- initialize info for get_multi_input
    char print_char_string[2]; // a string to store the print char
        // , which will then be converted into just a char
    (void) memset(print_char_string, 0, sizeof(print_char_string));

    // get_multi_input argument values
    int text_pos[3] = {1, 3, 5};
    // NOTE: accessing note.string like this shouldn't be done
    char *dest[3] 
        = {target_creature->name, print_char_string
            , target_creature->note.string};
    int max_lens[3] = {MAX_CHAR, 2, MAX_CHAR};

    (void) mvprintw(0, 0, "Name: ");
    (void) mvprintw(2, 0, "Char: ");
    (void) mvprintw(4, 0, "Notes: ");

    // ----- get information about the character
    (void) get_multi_input(stdscr, dest, 3, max_lens, text_pos);

    // ----- convert the print_char_string into a char and save it into
        // the creature
    target_creature->print_char = print_char_string[0];

    // ----- set creature inventory
    inventory_menu(target_campaign, NULL, target_creature);


    // ----- place the character on the grid
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
                    , &target_creature->name, PLACE_CHAR);
                goto end;
        }
    } while (c != KEY_F(2));


end:
    return 0;
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

        (void) get_multi_input(stdscr, pointer_array, 3, max_lens, text_pos);

        c = getch();

        // ----- deal with inventory
        inventory_menu(target_campaign, target_item, NULL);

        if (c == KEY_F(2)) {
            break;
        }
        (void) erase();
    }
    
    return 0;
}

void p_note_creation_menu(struct campaign *target_campaign
    , struct coord position)
    // theoretically works
{
    /*@null@*/ struct p_note *target_p_note = NULL;
    /*@null@*/ struct grid *target_grid = NULL;
    
    target_grid = &target_campaign->encounter_grid;
    target_p_note = &target_grid
        ->p_notes[target_grid->p_note_next_empty++];

    if (target_p_note == NULL) {
        return;
    }
    // set the coordinates
    target_p_note->x = position.x;
    target_p_note->y = position.y;

    char *pointer_array[1] = {target_p_note->note.string};
    int text_pos[1] = {1};
    int max_lens[1] = {target_p_note->note.len};

    (void) mvprintw(0, 0, "Notes");
    (void) get_multi_input(stdscr, pointer_array, 1, max_lens, text_pos);

    (void) erase();
}

void material_creation_menu(struct campaign *target_campaign)
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
    /*@null@*/ struct material *target_material = NULL;
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

        // ----- get user input for text
        target_material = &target_campaign->material_list
            [target_campaign->next_empty_material++];

        char print_char_string[2];
        (void) memset(print_char_string, 0, sizeof(*print_char_string));

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
        // prompt the user
        c = wgetch(ui.main_win);

        if (c != KEY_F(2)) {
            break;
        }
        (void) werase(ui.main_win);
    }

    del_ui(&ui);
}
