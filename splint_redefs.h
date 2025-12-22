#include <panel.h>
#include <menu.h>

// splint redefinitions of ncurses functions with proper annotations
#ifdef S_SPLINT_S
    extern int wattr_on(WINDOW *win, attr_t attrs
        , /*@null@*/ void *opts);
    extern int wattr_off(WINDOW *win, attr_t attrs
        , /*@null@*/ void *opts);
    extern /*@temp@*/ WINDOW *newwin(int nlines, int ncols
        , int begin_y, int begin_x);
    extern /*@dependent@*/ const void *panel_userptr(const PANEL *pan);
    extern int set_panel_userptr(PANEL *pan, /*@null@*/ const void *ptr);
    extern /*@temp@*/ WINDOW *derwin(WINDOW *orig, int nlines
        , int ncols, int begin_y, int begin_x);

    extern /*@only@*/ MENU *new_menu(/*@partial@*/ ITEM **items);
    extern /*@only@*/ ITEM *new_item(const char *name
        , const char *description);
    extern int free_item(/*@special@*/ ITEM *item) /*@releases item@*/;
    extern int free_menu(/*@special@*/ MENU *menu) /*@releases menu@*/;
    extern /*@dependent@*/ item_value(const ITEM *item);
    extern /*@dependent@*/ item_name(const ITEM *item);
    extern /*@dependent@*/ ITEM **menu_items(const MENU *menu);
    extern /*@dependent@*/ ITEM *current_item(const MENU *menu);
#endif
