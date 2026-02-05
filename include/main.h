// TODO modularize this when its too difficult
#include <stdio.h>
#include <stdlib.h>
#include "splint_redefs.h"
#include "dbg.h"
#include <string.h>
// TODO: rewrite dbg.h such that it can output in the final window
#define MAX_CHAR 50
#define MAX_NOTE 50 // max length for notes
#define ARRAY_LEN(a) (int) (sizeof(a) / sizeof(a[0]))
#define MAX_PNOTES 10
#define MAX_CREATURES 2
#define MAX_ITEMS 2
#define MAX_SAVED_CREATURES 10
#define MAX_SAVED_MATERIALS 10
#define MAX_SAVED_ITEMS 10
#define GRID_X 3
#define GRID_Y 2
#define PRINT_TABS(t) \
    {for (int p = 0; p < t; p++) (void) putchar('\t');}
#define MAX_INVENTORY 3
#define SQUARE_X_LEN 5
#define SQUARE_Y_LEN 5

#define MOVE_UP         0
#define MOVE_DOWN       1
#define MOVE_LEFT       2
#define MOVE_RIGHT      3

#define CURSOR_UP       4
#define CURSOR_DOWN     5
#define CURSOR_LEFT     6
#define CURSOR_RIGHT    7
#define SET_MAT         8
#define PLACE_CHAR      9
#define TOGGLE_WALL     10

struct func_pointer {
    void (*pointer) (void *);
    int type; // specifies a type of function pointer to convert to
};

struct coord {
    int y;
    int x;
};

// redundancy for later on if I want to implement a better note system
struct note {
    char string[MAX_NOTE];
    int len;
};

struct creature {
    char name[MAX_CHAR];
    int name_len;
    char print_char;

    struct note note;
};

struct item {
    char name[MAX_CHAR];
    int name_len;
    char print_char;
    int weight;
    bool has_inventory;
    struct item *inventory[MAX_INVENTORY];
    int inventory_len;
    int num_inv_items; // number of items in the inventory

    struct note note;
};

typedef /*@null@*/ struct creature * pos_null_creature;
typedef /*@null@*/ struct item * pos_null_item;

struct square {
    /*@null@*/ /*@dependent@*/ struct material *material;
        // pointer to material from master list
    bool is_wall;
    pos_null_creature creatures[MAX_CREATURES]; // pointer to
        // creature from master list
    // NOTE: removing max_creatures can save space if needed
    int max_creatures; // maximum amount of creatures that can be added
    int num_creatures; // current number of creatures on this square

    pos_null_item items[MAX_ITEMS]; // pointer to items in master list
    int max_items;
    //int num_items; // include if neccessary
    int movement_modifier; // usually 1 if in difficult terrain
};

struct p_note {
    int x;
    int y;
    
    struct note note;
};

struct material {
    char name[MAX_CHAR];
    char desc[MAX_CHAR];
    char print_char;

    struct note note;
};

struct grid {
    struct square squares[GRID_Y][GRID_X];
        // coordinates are going to be like ncurses
    int max_y;
    int max_x;

    struct coord grid_start;

    struct p_note p_notes[MAX_PNOTES];
    int p_note_len;

    int x_scale; // the ft that a square is (normally 5ft)
    int y_scale;

    // TODO maybe add a diagonal scale for moving diagonally
};

struct campaign {
    char name[MAX_CHAR];

    struct grid encounter_grid;
    // encounter grid array??

    struct note note;

    struct material material_list[MAX_SAVED_MATERIALS];
    int material_list_len;
    //int next_empty_material;
    struct creature creature_list[MAX_SAVED_CREATURES];
    int creature_list_len;
    //int next_empty_creature;
    struct item item_list[MAX_SAVED_ITEMS];
    int item_list_len;
    int next_empty_item; // points to the index of the next empty item
};

struct grid_editor {
    /*@temp@*/ struct grid *target_grid;
    /*@temp@*/ WINDOW *target_win;

    struct coord cursor;
    struct coord max_cursor;
    struct coord old_cursor;
    struct coord grid_end;
};
typedef struct grid_editor GRID_EDITOR;

int init_campaign(/*@out@*/ struct campaign *target);
int init_grid(/*@out@*/ struct grid *target);
int init_square(/*@out@*/ struct square *target);
int init_material(/*@out@*/ struct material *target);
int init_p_note(/*@out@*/ struct p_note *target);
int init_note(/*@out@*/ struct note *target);
void get_note(struct note *target
    , char *buffer, int buffer_len);
void print_note(struct note *target, FILE *format);
void debug_p_note(struct p_note *target, int tabs, FILE *format);
void debug_campaign(struct campaign *target, FILE *format);
void debug_grid(struct grid *target, int tabs, FILE *format);
void debug_square(struct square *target, int tabs, FILE *format);
void debug_material(/*@null@*/ struct material *target
    , int tabs, FILE *format);
int start_menu();
int start_campaign(struct campaign *target_campaign);
int get_multi_input(char **dest, int num_dest, int *buffer_lens
    , int *text_pos);
int start_encounter(struct campaign *target_campaign);
void mvprintw_square(int y, int x, struct square *target
    , WINDOW *restrict window);
int creature_creation_menu(struct campaign *target_campaign);
int init_creature(/*@out@*/ struct creature *target);
int print_grid(struct grid *target_grid, WINDOW *target_window
    , struct coord start_point, struct coord end_point);
void debug_creature(/*@null@*/ struct creature *target
    , int tabs, FILE *format);
int grid_editor_driver(GRID_EDITOR *target_ge
    , /*@null@*/ /*@dependent@*/ void *target_information
    , const int action);
int init_grid_editor(/*@out@*/GRID_EDITOR *target_ge
    , struct grid *target_grid
    , struct coord grid_end, WINDOW *target_win);
int main_menu(struct campaign *target_campaign);
void mvdisplay_square_info(WINDOW *target_window, int y, int x
    , struct square *target_square);
struct coord get_cursor(GRID_EDITOR *target);
void save_grid_ptrs(struct campaign *target_campaign
    , FILE *restrict material_file
    , FILE *restrict creature_file);
void load_grid_material_ptrs(struct campaign *target_campaign
    , FILE *restrict material_file);
void load_grid_creature_ptrs(struct campaign *target_campaign
    , FILE *restrict material_file);
void init_item(/*@out@*/ struct item *target);
void debug_item(/*@null@*/ struct item *target, int tabs, FILE *format);
int item_creation_menu(struct campaign *target_campaign);
void item_inventory_menu(struct campaign *target_campaign
    , struct item *target_item);
