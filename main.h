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
#define MAX_CREATURES 10
#define GRID_X 100
#define GRID_Y 100
#define PRINT_TABS(t) \
    {for (int p = 0; p < t; p++) (void) putchar('\t');}

struct func_pointer {
    void (*pointer) (void *);
    int type; // specifies a type of function pointer to convert to
};

// redundancy for later on if I want to implement a better note system
struct note {
    char string[MAX_NOTE];
    int len;
};

struct creature {
    char name[MAX_CHAR];
    int name_len;

    struct note note;
};

struct square {
    /*@null@*/ /*@dependent@*/ struct material *material;
        // pointer to material from master list
    bool is_wall;
    struct creature *creatures[MAX_CREATURES]; // pointer to
        // creature from master list
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
};

struct coord {
    int y;
    int x;
};

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
int get_multi_input(char **dest, int num_dest, int max_buffer_len, int *text_pos);
int start_encounter(struct grid *target_grid
    , struct material *mat_list, int mat_list_len);
void mvprintw_square(int y, int x, struct square *target
    , WINDOW *restrict window);
int creature_creation_menu(struct creature *creature_list
    , int creature_list_len);
int init_creature(/*@out@*/ struct creature *target);
int print_grid(struct grid *target_grid, WINDOW *target_window
    , struct coord start_point, struct coord end_point);
