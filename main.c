#include "main.h"

static void encounter_debug();
static void creature_debug();
static void save_debug();

int main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    encounter_debug();
    return 0;
}

/*@unused@*/ static void creature_debug()
{
    struct creature *mast_creature_list;
    int len = 5;
    mast_creature_list = calloc((size_t) len 
        , sizeof(*mast_creature_list));

    if (mast_creature_list == NULL) {
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < len; i++) {
        (void) init_creature(&mast_creature_list[i]);
    }
    (void) snprintf(mast_creature_list[0].name, MAX_CHAR, "Hi");

    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);

    (void) creature_creation_menu(mast_creature_list, len);

    (void) endwin();
    free(mast_creature_list);
}

/*@unused@*/ static void encounter_debug()
{
    struct material *mat_master_list;
    int mat_master_len = 5;
    mat_master_list = calloc((size_t) mat_master_len
        , sizeof(*mat_master_list));

    if (mat_master_list == NULL) {
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < mat_master_len; i++) {
        (void) init_material(&mat_master_list[i]);
    }
    // ----- start ncurses
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);
    // ----- start menu
    struct campaign temp;
    (void) init_campaign(&temp);
    //(void) start_campaign(&temp);
    (void) snprintf(mat_master_list[0].name, MAX_CHAR, "ground");
    (void) snprintf(mat_master_list[0].desc, MAX_CHAR, "this is gorund");
    mat_master_list[0].print_char = '_';

    (void) snprintf(mat_master_list[1].name, MAX_CHAR, "vegitation");
    (void) snprintf(mat_master_list[1].desc, MAX_CHAR, "this is veg");
    mat_master_list[1].print_char = '"';

    (void) start_encounter(&temp.encounter_grid
        , mat_master_list, mat_master_len);

    (void) endwin();
    //debug_campaign(&temp, stdout);
    free(mat_master_list);
}

/*@unused@*/ static void save_debug()
{
    /*@null@*/ FILE *save_file = NULL;
    struct campaign test;
    struct campaign test2;

    (void) memset(&test, 0, sizeof(test));

    (void) init_campaign(&test);

    save_file = fopen("test", "w+");
    if (save_file == NULL) {
        exit(EXIT_FAILURE);
    }

    // saving
    (void) fwrite(&test, sizeof(test), 1, save_file);

    // reading (rewind is to reset the file pos
        // indicator to 0 again
    rewind(save_file);
    (void) fread(&test2, sizeof(test2), 1, save_file);

    debug_campaign(&test2, stdout);

    (void) fclose(save_file);
}
