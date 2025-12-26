#include "main.h"

int init_grid(/*@out@*/ struct grid *target)
    // initializes a grid
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

    // -- set scale values to default (5)
    target->x_scale = 5;
    target->y_scale = 5;

    return 0;
error:
    return -1;
}

int init_square(/*@out@*/ struct square *target)
    // initializes a square
    // ----- returns -----
    // 0: normal
    // -1: error
{
    // ----- checks
    check(target != NULL, "target was NULL (should be square to"
        " initialize).");

    // ----- set/initalize all values to their defaults
    target->material = NULL;
    target->is_wall = false;
    (void) memset(target->creatures, 0, sizeof(target->creatures));
    target->movement_modifier = 0;

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
        (void) puts("");
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
        fprintf(format, "is_wall: %d | movement_modifier: %d\n"
            , target->is_wall ? 1 : 0, target->movement_modifier);
        debug_material(target->material, tabs + 1, format);
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
        " | max_x: %d | p_note_len: %d\n"
        , target->x_scale, target->y_scale
        , target->max_y, target->max_x, target->p_note_len);

    // print the p_notes
    for (int i = 0; i < target->p_note_len; i++) {
        debug_p_note(&target->p_notes[i], tabs + 1, format);
    }
    (void) puts("");
    
    // print all the squares
    for (int i = 0; i < target->max_y; i++) {
        for (int j = 0; j < target->max_x; j++) {
            debug_square(&target->squares[i][j], tabs + 1, format);
        }
    }
    // check the squares and print them

}
