// TODO: test material creation
#include "../include/main.h"

static void menus_debug();
static void creature_debug();
static void save_debug();
static void load_debug();
static void test_debug();
static void application();

int main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    // TODO change integer returns to void if not tracked
    // TODO finish adding commands_win
    application();
    return 0;
}

/*@unused@*/ static void application()
    // this is the actual application
{
    struct campaign main_campaign;
    (void) init_campaign(&main_campaign);

    int function_select = -1;
    int function_output = 0;

    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);

    // ----- start menu
    function_select = start_menu();

    // ----- select and run a function
    function_output
        = run_main_menu_function(function_select, &main_campaign);
    
    (void) endwin();

    //debug_campaign(&main_campaign, stdout);
}

/*@unused@*/ static void test_debug()
{
    struct campaign temp;
    (void) init_campaign(&temp);

    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);

    int c = item_creation_menu(&temp, NULL);

    (void) endwin();

    printf("return is %d\n", c);
    FILE *item_save_file = fopen("i.save", "w");

    if (item_save_file == NULL) {
        return;
    }

    //save_item_inventory(&temp, item_save_file);

    debug_campaign(&temp, stdout);
}

/*@unused@*/ static void creature_debug()
{
    struct campaign temp;
    (void) init_campaign(&temp);

    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);

    (void) creature_creation_menu(&temp, NULL, false);

    (void) endwin();
    debug_campaign(&temp, stdout);
}

/*@unused@*/ static void menus_debug()
{
    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);
    // ----- start menu
    struct campaign temp;
    (void) init_campaign(&temp);

    // ----- init materials
    //(void) snprintf(temp.material_list[0].name, MAX_CHAR, "ground");
    //(void) snprintf(temp.material_list[0].desc, MAX_CHAR, "this is gorund");
    //temp.material_list[0].print_char = '_';

    //(void) snprintf(temp.material_list[1].name, MAX_CHAR, "vegitation");
    //(void) snprintf(temp.material_list[1].desc, MAX_CHAR, "this is veg");
    //temp.material_list[1].print_char = '"';
    
    (void) material_creation_menu(&temp, NULL);
    (void) start_encounter(&temp);
    (void) creature_creation_menu(&temp, NULL, false);

    (void) main_menu(&temp);
    (void) endwin();

    //(void) fwrite(&temp, sizeof(temp), 1, save_file);

    debug_campaign(&temp, stdout);
}

/*@unused@*/ static void save_debug()
{
    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);

    struct campaign temp;
    (void) init_campaign(&temp);

    (void) snprintf(temp.material_list[0].name, MAX_CHAR, "ground");
    (void) snprintf(temp.material_list[0].desc, MAX_CHAR, "this is gorund");
    temp.material_list[0].print_char = '_';

    (void) snprintf(temp.material_list[1].name, MAX_CHAR, "vegitation");
    (void) snprintf(temp.material_list[1].desc, MAX_CHAR, "this is veg");
    temp.material_list[1].print_char = '"';

    //(void) start_encounter(&temp);
    //(void) creature_creation_menu(&temp);
    (void) endwin();

    //debug_campaign(&temp, stdout);

    // NOTE: keep opening and writing and closing files
        // seperate from the normal code, as it may cause a seg fault
        // from stream corruption??
    save_campaign(&temp);
}

/*@unused@*/ static void load_debug()
{
    struct campaign temp;
    (void) init_campaign(&temp);

    load_campaign(&temp);
    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);

    (void) main_menu(&temp);

    (void) endwin();

    debug_campaign(&temp, stdout);
}
