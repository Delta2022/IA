#include "main.h"
// TODO encapsulate note

int init_p_note(/*@out@*/ struct p_note *target)
    // initializes a grid
    // ----- returns -----
    // 0: normal
    // -1: error
{
    // ----- init
    int foutput = 0;

    // ----- checks
    check(target != NULL, "target was NULL (should be p_note to"
        " initialize).");

    // ----- set/initalize all values to their defaults
    target->x = 0;
    target->y = 0;

    foutput = init_note(&target->note);
    check(foutput == 0, "init_note failed with code %d", foutput);

    return 0;
error:
    return -1;
}

int init_note(/*@out@*/ struct note *target)
    // initializes a note assumes the note is already memset to 0
    // ----- returns -----
    // 0: normal
    // -1: error
{
    // ----- checks
    check(target != NULL, "target was NULL (should be note to"
        " initialize).");

    // ----- set value
    //(void) memset(target->string, 0, sizeof(target->string));
    target->len = ARRAY_LEN(target->string);

    return 0;
error:
    return -1;
}

void get_note(struct note *target
    , char *buffer, int buffer_len)
    // puts the note's string into *buffer and returns the length
    // including the terminating null
    // puts <NULL> if null
{
    if (target == NULL) {
        // using snprintf due to the fact that it is always
        // null terminated
        (void) snprintf(buffer, (size_t) buffer_len, "<NULL>");
    } else if (target->string == NULL) {
        (void) snprintf(buffer, (size_t) buffer_len
            , "<string is NULL (shouldn't happen)>");
    } else {
        // ----- put the string of target only if target isn't null
        //(void) strncpy(buffer, target->string, buffer_len);
        //buffer[buffer_len - 1] = '\0';

        (void) snprintf(buffer, (size_t) buffer_len
            , "\"%s\"", target->string);
    }
}

void print_note(struct note *target, FILE *format)
    // prints a note's string with no \n
{
    if (target == NULL) {
        fprintf(format, "<NULL>");
    } else if (target->string == NULL) {
        fprintf(format, "<string is NULL (shouldn't happen)>");
    } else {
        fprintf(format, "\"%s\"", target->string);
    }
}

void debug_p_note(struct p_note *target, int tabs, FILE *format)
    // prints a p_note
{

    PRINT_TABS(tabs);
    fprintf(format, "P_NOTE | ");
    if (target == NULL) { // print a message if the target is NULL
        (void) fprintf(format, "<NULL>\n");
    } else {
        fprintf(format, "(y: %d, x: %d) ", target->y, target->x);
        print_note(&target->note, format);
        (void) fprintf(format, "\n");
    }
}

