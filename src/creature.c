#include "../include/main.h"

int init_creature(/*@out@*/ struct creature *target)
    // inits a creature
    // assumes target is already memset to 0 beforehand
{
    if (target == NULL) {
        return -1;
    }

    target->name_len = ARRAY_LEN(target->name);
    //(void) memset(target->name , 0, sizeof(target->name_len));
    //target->print_char = '\0';
    target->inventory_len = ARRAY_LEN(target->inventory);

    (void) init_note(&target->note);

    return 0;
}

// TODO switch all puts to fputs
void debug_creature(/*@null@*/ struct creature *target, int tabs
    , FILE *format)
{
    PRINT_TABS(tabs);
    fprintf(format, "CREATURE | ");
    if (target == NULL) {
        fprintf(format, "<NULL>\n");
        return;
    }
    fprintf(format
        , "name: \"%s\" | name_len: %d | print_char: %c | note: "
        , target->name, target->name_len, target->print_char);
    print_note(&target->note, format);

    (void) fprintf(format, "\n");
    for (int i = 0; i < target->inventory_len; i++) {
        debug_item(target->inventory[i], tabs + 1, format);
    }

}
