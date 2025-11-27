#include <menu.h>
#include <stdlib.h>
#include <string.h>

#ifdef S_SPLINT_S
extern int free_item(/*@special@*/ ITEM *item) /*@releases item@*/;
extern int free_menu(/*@special@*/ MENU *menu) /*@releases menu@*/;
#endif

int main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    /*@unused@*/ MENU *my_menu;
    ITEM *my_item[3];
    char string1[] = "hi";
    char string2[] = "bye";
    
    (void) initscr();

    (void) memset(my_item, 0, sizeof(my_item)); // sizeof(array)
        // gives the size of the entire array not just the pointer
    
    my_item[0] = new_item(string1, string1);
    my_item[1] = new_item(string2, string2);
    my_item[2] = NULL;

    my_menu = new_menu(my_item);

    (void) free_menu(my_menu);
    (void) free_item(my_item[0]);
    (void) free_item(my_item[1]);

    (void) endwin();
    return 0;
}
