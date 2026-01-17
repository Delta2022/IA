#include "main.h"

int init_grid_editor(/*@out@*/GRID_EDITOR *target_ge
    , struct grid *target_grid
    , struct coord grid_end, WINDOW *target_win)
    // define the starting values for a grid editor based on the
    // target_grid
    // grid_end is the point that the grid should stop printing
{
    // ----- define cleaner variables to use
    struct coord *cursor = &target_ge->cursor;
    struct coord *old_cursor = &target_ge->old_cursor;

    struct coord *max_cursor = &target_ge->max_cursor;

    struct coord *grid_start = &target_grid->grid_start;
    target_ge->grid_end = grid_end;

    (void) print_grid(target_grid, target_win
        , *grid_start, grid_end);

    // ----- set inital values
    *cursor = *grid_start;

    // --- set max_cursor to either the end of the grid (target_grid->max_)
        // OR the given end point to print the grid (grid_end)
    max_cursor->y = target_grid->max_y - 1; 
        // - 1 is to convert length to index
    max_cursor->x = target_grid->max_x - 1;

    // set the *max_cursor to the end of the grid if it
        // is larger than the end of the grid
    if (max_cursor->y > grid_end.y) {
        max_cursor->y = grid_end.y;
    }
        
    if (max_cursor->x > grid_end.x - 1) { // -1 due to grid_end being
        // length
        max_cursor->x = grid_end.x - 1;
    }

    *old_cursor = *cursor; // set the starting old_cursor
        // to the starting *cursor as a default
        // (they are the same, so its caught when showing)

    target_ge->target_grid = target_grid;
    target_ge->target_win = target_win;
    return 0;
}

int grid_editor_driver(GRID_EDITOR *target_ge
    , /*@null@*/ /*@dependent@*/ void *target_information
    , const int action)
    // target_information will be a pointer to a character if the action
    // is PLACE_CHAR and a pointer to material if the action is SET_MAT.
    // it is otherwise ignored and can be NULL
{
    // ----- set cleaner variables to use
    WINDOW *target_win = target_ge->target_win;
    struct grid *target_grid = target_ge->target_grid;

    struct coord *cursor = &target_ge->cursor;
    struct coord *old_cursor = &target_ge->old_cursor;

    struct coord *max_cursor = &target_ge->max_cursor;

    struct coord *grid_start = &target_grid->grid_start;
    struct coord *grid_end = &target_ge->grid_end;

    struct square *cursor_square;

    struct material *target_material;
    struct creature *target_creature;
    /*@null@*/ int *next_empty_creature = NULL;

    // ----- do the action
    switch (action) {
        // ----- move the grid
        case MOVE_DOWN:
            grid_start->y++;
            cursor->y++;
            (void) print_grid(target_grid, target_win
                , *grid_start, *grid_end);
            break;
        case MOVE_UP:
            grid_start->y--;
            cursor->y--;
            (void) print_grid(target_grid, target_win
                , *grid_start, *grid_end);
            break;
        case MOVE_LEFT:
            grid_start->x++;
            cursor->x++;
            (void) print_grid(target_grid, target_win
                , *grid_start, *grid_end);
            break;
        case MOVE_RIGHT:
            grid_start->x--;
            cursor->x--;
            (void) print_grid(target_grid, target_win
                , *grid_start, *grid_end);
            break;

        // ----- move the cursor
            // cursor - grid_start is the coordinates on the grid
                // irrespective of movement offsets
            // the if statements ensure that the cursor
                // dont exceed the grid
        case CURSOR_UP:
            if (cursor->y - grid_start->y > 0) {
                cursor->y--;
            }
            break;
        case CURSOR_DOWN:
            if (cursor->y - grid_start->y < max_cursor->y) {
                cursor->y++;
            }
            break;
        case CURSOR_LEFT:
            if (cursor->x - grid_start->x > 0) {
                cursor->x--;
            }
            break;
        case CURSOR_RIGHT:
            if (cursor->x - grid_start->x < max_cursor->x) {
                cursor->x++;
            }
            break;

        // ----- set material
        case SET_MAT:
            target_material = (struct material *)target_information;
            if (target_material == NULL) {
                break;
            }

            cursor_square = &target_grid->squares
                [cursor->y - grid_start->y][cursor->x - grid_start->x];
            // cursor - *grid_start is to ensure that the offsets 
                // when moving around are cancelled out

            cursor_square->material = target_material;
            
            // update the square under the cursor
            mvprintw_square(cursor->y, cursor->x
                , cursor_square, target_win);
            break;

        // ----- place character
        case PLACE_CHAR:
            next_empty_creature = NULL;
            target_creature = (struct creature *)target_information;
            if (target_creature == NULL) {
                break;
            }

            cursor_square = &target_grid->squares
                [cursor->y - grid_start->y][cursor->x - grid_start->x];

            // --- find the next empty place in the square
                // to store the creature
            next_empty_creature = &cursor_square->num_creatures;

            // if no place is found for the creature
                // NOTE: prob needs to return a proper value
            if (*next_empty_creature == cursor_square->max_creatures) {
                break;
            }

            // --- save the creature's pointer into the square
            cursor_square->creatures[*next_empty_creature]
                = target_creature;

            // --- update the square's creature count
            (*next_empty_creature)++;

            // --- update the square
            mvprintw_square(cursor->y, cursor->x
                , cursor_square, target_win);

            break; 
        
        // ----- set wall
        case TOGGLE_WALL:
            // debug
            //(void) mvwprintw(target_win, LINES - 3, 0
            //    , "set target_grid->squares[%d][%d]"
            //    , cursor->y - grid_start->y, cursor->x - grid_start->x);
            cursor_square = 
                &target_grid->squares[cursor->y - grid_start->y]
                    [cursor->x - grid_start->x];

            cursor_square->is_wall = !(cursor_square->is_wall);
            mvprintw_square(cursor->y, cursor->x
                , cursor_square, target_win);
            break;
    }

    // ----- update the grid
    mvwchgat(target_win
        , cursor->y, cursor->x, 1, A_REVERSE, 0, NULL);

    // only replace the old position with normal
        // if the cursor was moved
    if (cursor->y != old_cursor->y || cursor->x != old_cursor->x) {
        mvwchgat(target_win , old_cursor->y, old_cursor->x
            , 1, A_NORMAL, 0, NULL);
        *old_cursor = *cursor; // copy the new cursor into the old
    }

    // debug
    //(void) mvwprintw(target_win, LINES - 2, 0, "cursor (%d %d)"
    //    ", max_cursor (%d %d)"
    //    ", old_cursor (%d %d), grid_end (%d %d)"
    //    , cursor->y, cursor->x
    //    , max_cursor->y, max_cursor->x
    //    , old_cursor->y, old_cursor->x
    //    , grid_end->y, grid_end->x);
    //(void) mvwprintw(target_win, LINES - 1, 0, "grid_start (%d %d)"
    //    , grid_start->y, grid_start->x);

    return 0;
}

struct coord get_cursor(GRID_EDITOR *target)
    // returns the cursor position of the target in the grid (not where
    // it is on screen)
{
    struct coord *cursor = &target->cursor;
    struct coord *grid_start = &target->target_grid->grid_start;
    struct coord return_val = {cursor->y - grid_start->y
        , cursor->x - grid_start->x};
    return return_val;
}
