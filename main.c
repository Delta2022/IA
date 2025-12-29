#include "main.h"



int main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    // TODO define the function array
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
    debug_campaign(&temp, stdout);
    free(mat_master_list);
    return 0;
}
