#include "main.h"

int init_creature(/*@out@*/ struct creature *target)
{
    if (target == NULL) {
        return -1;
    }

    target->name_len = ARRAY_LEN(target->name);
    (void) memset(target->name , 0, sizeof(target->name_len));

    (void) init_note(&target->note);

    return 0;
}
