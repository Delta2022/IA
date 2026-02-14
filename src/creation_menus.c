#include "../include/main.h"

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

        // ----- deal with inventory
        item_inventory_menu(target_campaign, target_item);

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
    (void) get_multi_input(pointer_array, 1, max_lens, text_pos);

    (void) erase();
}

void material_creation_menu(struct campaign *target_campaign)
{
    /*@null@*/ struct material *target_material = NULL;
    int c = 0;

    while (true) {
        // check if there is space
        if (target_campaign->next_empty_material
            == target_campaign->material_list_len) {
            break;
        }

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

        (void) mvprintw(0, 0, "Name");
        (void) mvprintw(2, 0, "Description");
        (void) mvprintw(4, 0, "Print character");
        (void) mvprintw(6, 0, "Notes");

        (void) get_multi_input(pointer_array, 4, max_lens, text_pos);

        // move convert print_char_string from a string into a character
        target_material->print_char = print_char_string[0];

        // prompt the user
        c = getch();

        if (c == KEY_F(2)) {
            break;
        }
    }

    (void) erase();
}
