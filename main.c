#include "main.h"

static void menus_debug();
static void creature_debug();
static void save_debug();
static void load_debug();

int main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    // TODO change integer returns to void if not tracked
    load_debug();
    return 0;
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

    (void) creature_creation_menu(&temp);

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
    (void) snprintf(temp.material_list[0].name, MAX_CHAR, "ground");
    (void) snprintf(temp.material_list[0].desc, MAX_CHAR, "this is gorund");
    temp.material_list[0].print_char = '_';

    (void) snprintf(temp.material_list[1].name, MAX_CHAR, "vegitation");
    (void) snprintf(temp.material_list[1].desc, MAX_CHAR, "this is veg");
    temp.material_list[1].print_char = '"';
    
    (void) start_encounter(&temp);
    (void) creature_creation_menu(&temp);

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

    (void) start_encounter(&temp);
    (void) creature_creation_menu(&temp);
    (void) endwin();

    debug_campaign(&temp, stdout);

    // NOTE: keep opening and writing and closing files
        // seperate from the normal code, as it may cause a seg fault
        // from stream corruption??
    FILE *c_save_file = fopen("c.save", "w");
    FILE *mat_save_file = fopen("m.save", "w");
    FILE *save_file = fopen("save.save", "w");
    if (c_save_file == NULL) {
        exit(EXIT_FAILURE);
    }
    if (mat_save_file == NULL) {
        exit(EXIT_FAILURE);
    }
    if (save_file == NULL) {
        exit(EXIT_FAILURE);
    }

    // save the data
    // TODO make saving not save the old pointers
    save_grid_ptrs(&temp, mat_save_file, c_save_file);
    (void) fwrite(&temp, sizeof(temp), 1, save_file);

    (void) fclose(mat_save_file);
    (void) fclose(c_save_file);
    (void) fclose(save_file);
}

/*@unused@*/ static void load_debug()
{
    struct campaign temp;
    (void) init_campaign(&temp);

    FILE *c_save_file = fopen("c.save", "r");
    FILE *mat_save_file = fopen("m.save", "r");
    FILE *save_file = fopen("save.save", "r");
    if (c_save_file == NULL) {
        exit(EXIT_FAILURE);
    }
    if (mat_save_file == NULL) {
        exit(EXIT_FAILURE);
    }
    if (save_file == NULL) {
        exit(EXIT_FAILURE);
    }
    
    // ----- read from file
    (void) fread(&temp, sizeof(temp), 1, save_file);
    printf("----- materials\n");
    load_grid_material_ptrs(&temp, mat_save_file);
    printf("----- creatures\n");
    load_grid_creature_ptrs(&temp, c_save_file);

    (void) fclose(mat_save_file);
    (void) fclose(c_save_file);
    (void) fclose(save_file);

    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);

    (void) main_menu(&temp);

    (void) endwin();

    debug_campaign(&temp, stdout);
}
