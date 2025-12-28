#include "main.h"

int init_campaign(/*@out@*/ struct campaign *target)
    // initializes a campaign
    // ----- returns -----
    // 0: normal
    // -1: error
{
    // ----- checks
    check(target != NULL, "target was NULL (should be campaign to"
        " initialize).");
    // ----- init
    int foutput = 0;

    // ----- set/initialize all values to their defaults
    (void) memset(target->name, 0, sizeof(target->name));

    foutput = init_grid(&target->encounter_grid);
    check(foutput == 0, "init_grid failed with code %d", foutput);

    foutput = init_note(&target->note);
    check(foutput == 0, "init_note failed with code %d", foutput);

    return 0;

error:
    return -1;
}

void debug_campaign(struct campaign *target, FILE *format)
// TODO: implement {} and tabs by passing a var for the number of tabs
    //to print
{
    fprintf(format, "CAMPAIGN | ");
    if (target == NULL) {
        fprintf(format, "<NULL>\n");
    } else {
        fprintf(format, "name: '%s' | note: ", target->name);
        print_note(&target->note, format);
        (void) puts("");
        debug_grid(&target->encounter_grid, 1, format);
            // print 1 tab for grid
        (void) puts("");
    }
}
