#include <menu.h>
#include <stdlib.h>

// ----- splint definitions
# ifdef S_SPLINT_S
extern /*@only@*/ MENU *new_menu(/*@partial@*/ ITEM **items);
extern /*@only@*/ ITEM *new_item(const char *name
    , const char *description);
extern int free_item(/*@special@*/ ITEM *item) /*@releases item@*/;
extern int free_menu(/*@special@*/ MENU *menu) /*@releases menu@*/;

# endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4

static char *choices[] = {
    "Choice 1",
    "Choice 2",
    "Choice 3",
    "Choice 4",
    "Exit"
};

int main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    ITEM **my_items;
    /*@unused@*/ int c;
    MENU *my_menu;
    size_t n_choices = 0;
    /*@unused@*/ ITEM *cur_item;

    // ----- init
    (void) initscr();
    (void) cbreak();
    (void) noecho();
    (void) keypad(stdscr, true);

    // ----- setup the starting output
    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **) calloc(n_choices + 1, sizeof(ITEM *));
        // why n_choices + 1 -> allows the last item to be null

    if (my_items == NULL) {
        printf("ERROR: my_items was NULL");
        goto end;
    }

    for (int i = 0; i < (int) n_choices; i++) {
        my_items[i] = new_item(choices[i], choices[i]); // asks
            // for name and description
    }
    
    my_items[n_choices] = (ITEM *) NULL; // sets the last item to null

    my_menu = new_menu((ITEM **) my_items);
    (void) mvprintw(LINES - 2, 0, "F1 to Exit");
    (void) post_menu(my_menu);

    (void) refresh();

    // ----- input loop
    while ((c = getch()) != KEY_F(1)) {
        switch(c) {
            case KEY_DOWN:
                (void) menu_driver(my_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                (void) menu_driver(my_menu, REQ_UP_ITEM);
                break;
        }
    }

    // free everyting
    (void) unpost_menu(my_menu);
    (void) free_menu(my_menu);

    // frees every item in my_items
    for (int i = 0; i < (int) n_choices; i++) {
        (void) free_item(my_items[i]);
    }

    (void) free(my_items);

end:
    (void) endwin();
    return 0;
}
