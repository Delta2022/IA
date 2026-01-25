#include "main.h"

void init_item(/*@out@*/ struct item *target)
    // assumes the target is set to null beforehand
{
    // ----- checks
    check(target != NULL, "target was NULL (should be item to"
        " initialize).");

    // ----- init
    target->name_len = ARRAY_LEN(target->name);
    target->inventory_len = ARRAY_LEN(target->inventory);

    //target->name = memset(target->name, 0, (size_t)name_len);
    //target->inventory = memset(target->inventory, 0
        //, (size_t)inventory_len);
    //target->print_char = '\0';
    //target->weight = 0;

    (void) init_note(&target->note);

error:
    return;
}

void debug_item(/*@null@*/ struct item *target, int tabs, FILE *format)
    // prints an item
{
    PRINT_TABS(tabs);
    fprintf(format, "ITEM | ");
    if (target == NULL) {
        fprintf(format, "<NULL>\n");
        return;
    }

    fprintf(format, "name: \"%s\" | name_len: %d | print_char: %c"
        "| weight: %d | inventory_len: %d | note: "
        , target->name, target->name_len, target->print_char
        , target->weight, target->inventory_len);
    print_note(&target->note, format);

    fprintf(format, "\n");

    for (int i = 0; i < target->inventory_len; i++) {
        // TODO not complete
        debug_item(target->inventory[i], tabs + 1, format);
    }
}
