#include "main.h"

int init_grid(/*@out@*/ struct grid *target)
    // initializes a grid. Assumes the grid was memset
        // to zero already
    // ----- returns -----
    // 0: normal
    // -1: error
{
    // ----- checks
    check(target != NULL, "target was NULL (should be grid to"
        " initialize).");

    // ----- init
    int foutput = 0;
    // ----- set/initalize all values to their defaults
    target->max_y = ARRAY_LEN(target->squares);
    target->max_x = ARRAY_LEN(target->squares[0]);

    //target->grid_start.y = 0;
    //target->grid_start.x = 0;

    for (int i = 0; i < target->max_y; i++) {
        for (int j = 0; j < target->max_x; j++) {
            foutput = init_square(&target->squares[i][j]);
            check(foutput == 0
                , "init_square failed with code %d", foutput);
        }
    }


    // -- initalize all p_notes
    target->p_note_len = ARRAY_LEN(target->p_notes);
    for (int i = 0; i < target->p_note_len; i++) {
        foutput = init_p_note(&target->p_notes[i]);
        check(foutput == 0, "init_p_note failed with code %d", foutput);
    }

    // -- set scale values to default (should be 5ft * 5ft)
    target->x_scale = SQUARE_X_LEN;
    target->y_scale = SQUARE_Y_LEN;

    return 0;
error:
    return -1;
}

int init_square(/*@out@*/ struct square *target)
    // initializes a square
        // assumes the square was memset to 0 already
    // ----- returns -----
    // 0: normal
    // -1: error
{
    // ----- checks
    check(target != NULL, "target was NULL (should be square to"
        " initialize).");

    // ----- set/initalize all values to their defaults
    //target->material = NULL;
    target->is_wall = false;
    //(void) memset(target->creatures, 0, sizeof(target->creatures));
    target->max_creatures = ARRAY_LEN(target->creatures);
    //target->movement_modifier = 0;
    //target->num_creatures = 0;

    return 0;
error:
    return -1;
}

int init_material(/*@out@*/ struct material *target)
    // initializes a material 
    // ----- returns -----
    // 0: normal
    // -1: error
{
    // ----- checks
    check(target != NULL, "target was NULL (should be material to"
        " initialize).");
    
    // ----- init
    int foutput = 0;
    
    // ----- set/initalize all values to their defaults
    (void) memset(target->name, 0, sizeof(target->name));
    (void) memset(target->desc, 0, sizeof(target->desc));
    target->print_char = '\0';

    foutput = init_note(&target->note);
    check(foutput == 0, "init_note failed with code %d", foutput);

    return 0;
error:
    return -1;
}

void debug_material(/*@null@*/ struct material *target
    , int tabs, FILE *format)
    // prints the material's name, desc and note
{
    PRINT_TABS(tabs);
    fprintf(format, "material - ");
    if (target == NULL) {
        fprintf(format, "<NULL>\n");
    } else {
        fprintf(format, "name: \"%s\" | desc: \"%s\""
            " | char: '%c' | note: "
            , target->name, target->desc, target->print_char);
        print_note(&target->note, format);
        (void) fprintf(format, "\n");
    }
}

void debug_square(struct square *target, int tabs, FILE *format)
    // prints a square's values
{
    PRINT_TABS(tabs);
    fprintf(format, "SQUARE | ");
    if (target == NULL) {
        fprintf(format, "<NULL>\n");
    } else {
        fprintf(format, "is_wall: %d | max_creatures: %d"
            " | num_creatures: %d | movement_modifier: %d\n"
            , target->is_wall ? 1 : 0, target->max_creatures
            , target->num_creatures, target->movement_modifier);
        debug_material(target->material, tabs + 1, format);
        (void) fprintf(format, "\n");
        for (int i = 0; i < target->max_creatures; i++) {
            debug_creature(target->creatures[i], tabs + 1, format);
        }
    }
    
}

void debug_grid(struct grid *target, int tabs, FILE *format)
    // prints a grid
{
    PRINT_TABS(tabs);
    fprintf(format, "GRID | ");

    if (target == NULL) {
        fprintf(format, "<NULL>\n");
        return;
    }
    fprintf(format, "x_scale: %d | y_scale: %d | max_y: %d"
        " | max_x: %d | grid_start: (%d, %d) | p_note_len: %d\n"
        , target->x_scale, target->y_scale
        , target->max_y, target->max_x
        , target->grid_start.y, target->grid_start.x
        , target->p_note_len);

    // print the p_notes
    for (int i = 0; i < target->p_note_len; i++) {
        debug_p_note(&target->p_notes[i], tabs + 1, format);
    }
    (void) fprintf(format, "\n");
    
    // print all the squares
    for (int i = 0; i < target->max_y; i++) {
        for (int j = 0; j < target->max_x; j++) {
            //printf("%d %d\n", i, j);
            debug_square(&target->squares[i][j], tabs + 1, format);
        }
    }
    (void) fprintf(format, "\n");

    // check the squares and print them

}

void mvprintw_square(int y, int x, struct square *target
    , WINDOW *restrict window)
    // prints a square to a position in a stream
    // TODO allow multi character movement
{
    char print_char = '\0';
    if (target->num_creatures > 1) {
        // NOTE: doesn't account for more than 9 but its prob fine
        print_char = (char) target->num_creatures + '0';
    } else if (target->num_creatures == 1 
        && target->creatures[0] != NULL) {
        print_char = target->creatures[0]->print_char;
    } else if (target->is_wall == true) { // render wall
        print_char = '+';
    } else if (target->material == NULL) {
        print_char = '.';
    } else { // render the right material
        print_char = target->material->print_char;
    }
    (void) mvwprintw(window, y, x, "%c", print_char);

}

int print_grid(struct grid *target_grid, WINDOW *target_window
    , struct coord start_point, struct coord end_point)
    // prints a grid starting at the start point, up until
    // the end of the grid
    // TODO can refactor to make it only print between start_point
        // and end_point
{
    (void) werase(target_window);
    /*
    (void) mvwprintw(target_window
        , LINES - 1, 0, "start_point: (%d %d) end_point: (%d %d)"
        , start_point.y, start_point.x
        , end_point.y, end_point.x);
    */

    struct coord print_pos = {0, 0};

    for (int i = 0; i < target_grid->max_y; i++) {
        for (int j = 0; j < target_grid->max_x; j++) {
            print_pos.y = start_point.y + i;
            print_pos.x = start_point.x + j;

            // delete when the value isn't shown when printing anyways
            if (print_pos.y < 0 || print_pos.x < 0
                || print_pos.y > end_point.y 
                || print_pos.x > end_point.x) {
                continue;
            }

            (void) mvprintw_square(print_pos.y, print_pos.x
                , &target_grid->squares[i][j]
                , target_window);
        }
    }
    return 0;    
}

void mvdisplay_square_info(WINDOW *target_window, int y, int x
    , struct square *target_square)
    // displays a square's data onto a window at position y, x
    // TODO handle too much data
{
    // ----- print creature information
    /*@null@*/ struct creature *target_creature = NULL;
    if (target_square->num_creatures > 0) {

        // loop through every num_creature
        for (int i = 0; i < target_square->num_creatures; i++) {
            target_creature = target_square->creatures[i];
            if (target_creature == NULL) continue;

            // print the character and name
            (void) mvwprintw(target_window, y + 1 + i, x, "%c - %s"
                , target_creature->print_char, target_creature->name);
        }
    }
    // ----- print material information
    // --- print information (material + wall) when the material is NULL
    if (target_square->material == NULL) {
        (void) mvwprintw(target_window, y, x, "Nothing %s"
            , target_square->is_wall ? "with a wall" : "");
        return;
    }

    // --- print information (material + wall) if the material isn't NULL
    (void) mvwprintw(target_window, y, x, "%s %s"
        , target_square->material->name
        , target_square->is_wall ? "wall" : "");
}


/*
int print_grid(struct grid *target_grid, WINDOW *target_window
    , struct coord win_end, struct coord start_print
    , struct coord grid_start, struct coord grid_end)
    // prints the grid out into a target window
    // assumes the target window is larger than the squares
    // TODO allow moving around the grid
    // TODO report status
{
    // end print is the position when to stop printing
        // as this can be either the end of the grid or the 
        // end of the window
    struct coord end_print = grid_end;

    if (end_print.y > win_end.y) {
        end_print.y = win_end.y;
    }

    if (end_print.x > win_end.x) {
        end_print.x = win_end.x;
    }

    (void) mvwprintw(target_window
        , LINES - 2, 0, "(%d %d) (%d %d) (%d %d) (%d %d)"
        , win_end.y, win_end.x
        , start_print.y, start_print.x
        , grid_start.y, grid_start.x
        , grid_end.y, grid_end.x);

    (void) mvwprintw(target_window
        , LINES - 1, 0, "(%d %d) (%d %d)"
        , start_print.y, start_print.x
        , end_print.y, end_print.x);

    // NOTE: didn't clear line, which caused bugs
    // NOTE: this line had the error bc its not starting at 0
    for (int i = grid_start.y; i < end_print.y; i++) {
        for (int j = grid_start.x; j < end_print.x; j++) {
            (void) mvprintw_square(start_print.y + i, start_print.x + j
                , &target_grid->squares[i]
                    [j]
                , target_window);
        }
    }
    return 0;
}
*/
