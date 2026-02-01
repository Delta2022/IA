#include <panel.h>
#include <menu.h>

// possibly null ITEM pointer
typedef /*@null@*/ ITEM * pos_null_ITEM_ptr;

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

    extern /*@null@*/ /*@only@*/ MENU *new_menu(
        /*@partial@*/ ITEM **items);
    extern /*@null@*/ /*@only@*/ ITEM *new_item(const char *name
        , const char *description);
    extern int free_item(/*@special@*/ ITEM *item) /*@releases item@*/;
    extern int free_menu(/*@special@*/ MENU *menu) /*@releases menu@*/;
    extern /*@dependent@*/ item_value(const ITEM *item);
    extern /*@dependent@*/ item_name(const ITEM *item);
    extern /*@dependent@*/ ITEM **menu_items(const MENU *menu);
    extern /*@dependent@*/ /*@null@*/ ITEM *current_item(const MENU *menu);
    extern wchgat(WINDOW *win, int n, attr_t attr, short pair
        , /*@null@*/ const void *opts);
    // NOTE: temp may be wrong here (see SPLINT_NOTE in menus.c)
    extern /*@null@*/ /*@temp@*/ ITEM *current_item(const MENU *menu);
    extern int set_menu_items(MENU *menu, pos_null_ITEM_ptr *items);
#endif
