#include "../include/main.h"
#include <math.h>
#include <float.h>
// test

#if S_SPLINT_S
    extern ssize_t getline(/*@out@*/ char **restrict lineptr
        , size_t *restrict n, FILE *restrict stream);
#endif

int init_campaign(/*@out@*/ struct campaign *target)
    // initializes a campaign
    // ----- returns -----
    // 0: normal
    // -1: error
{
    // ----- checks
    check(target != NULL, "target was NULL (should be campaign to initialize).");

    // ----- zero out the entire campaign
        // (needed for no valgrind errors)
    (void) memset(target, 0, sizeof(*target));
    // ----- init
    int foutput = 0;

    // ----- set/initialize all values to their defaults
    //(void) memset(target->name, 0, sizeof(target->name));

    foutput = init_grid(&target->encounter_grid);
    check(foutput == 0, "init_grid failed with code %d", foutput);

    foutput = init_note(&target->note);
    check(foutput == 0, "init_note failed with code %d", foutput);

    // --- define the lengths
    target->material_list_len = ARRAY_LEN(target->material_list);
    target->creature_list_len = ARRAY_LEN(target->creature_list);
    target->item_list_len = ARRAY_LEN(target->item_list);

    // --- init all creatures
    for (int i = 0; i < target->creature_list_len; i++) {
        (void) init_creature(&target->creature_list[i]);
    }

    // --- init all items
    for (int i = 0; i < target->item_list_len; i++) {
        init_item(&target->item_list[i]);
    }

    // --- init all materials
    for (int i = 0; i < target->material_list_len; i++) {
        (void) init_material(&target->material_list[i]);
    }

    return 0;

error:
    return -1;
}

void debug_campaign(struct campaign *target, FILE *format)
    // prints debug info for every camapgin
{
    fprintf(format, "CAMPAIGN | ");
    if (target == NULL) {
        fprintf(format, "<NULL>\n");
    } else {
        fprintf(format, "name: '%s' | creature_list_len: %d"
            " | material_list_len: %d | item_list_len: %d"
            " | next_empty_material: %d | next_empty_item: %d | note: "
            , target->name, target->creature_list_len
            , target->material_list_len, target->item_list_len
            , target->next_empty_material, target->next_empty_item);
        print_note(&target->note, format);
        
        (void) fprintf(format, "\n");

        debug_grid(&target->encounter_grid, 1, format);
            // print 1 tab for grid

        (void) fprintf(format, "\n");
        for (int i = 0; i < target->material_list_len; i++) {
            debug_material(&target->material_list[i], 1, format);
        }

        (void) fprintf(format, "\n");

        for (int i = 0; i < target->creature_list_len; i++) {
            debug_creature(&target->creature_list[i], 1, format);
        }

        (void) fprintf(format, "\n");

        for (int i = 0; i < target->item_list_len; i++) {
            debug_item(&target->item_list[i], 1, format);
        }
    }
}

typedef /*@null@*/ void * null_void_ptr;

// TODO; generalize by casting void and requesting array pos -> creature and item just run through loop
static int find_index(/*@null@*/ void *target_ptr, void *array_start_ptr
    , int array_len, size_t value_size)
    // returns the index of the target pointer in the array pointer
    // by using pointer arithmetic
    // NOTE: target_array must be an array of pointers to the positions
    // prints the index of the pointer in the array in a line in the
    // file
    // ALSO NOTE: this errors properly because I feel that this
        // is really easy to error with
    // ----- returns -----
    // 0 or positive: index of the pointer
    // -1: target_ptr is null
    // -2: calculated index isn't an integer (likely due to incorrect
        // input pointers)
    // -3: calculated index is larger than the length
        // of the array
{
    double index_val = -1;
    // print -1 when null
    if (target_ptr == NULL) {
        return -1;
    }

    // pointer arithmetic to the rescue
    index_val = (target_ptr - array_start_ptr) / (double) value_size;

    // checks
    // DBL_EPSILON is used since comparing to zero is dangerous
    if (index_val < 0 || index_val - floor(index_val) > DBL_EPSILON) {
        log_err("calculations resulted in an error."
            " (index calculations for %p and %p with size %d gives"
            " an index of %f which isn't an integer when it should)\n"
            , target_ptr, array_start_ptr, value_size, index_val);
        return -2;
    } else if ((int) index_val >= array_len) {
        log_err("index calculated (%d) was bigger than array length"
            " (%d)\n", (int) index_val, array_len);
        return -3;
    }

    return (int) index_val;
}

void save_grid_ptrs(struct campaign *target_campaign
    , FILE *restrict material_file
    , FILE *restrict creature_file)
    // saves the material and creatures on a grid by saving its
    // NOTE: this function can detect errors
    // index into a file
    // TODO do proper error handling
{
    struct grid *target_grid = &target_campaign->encounter_grid;
    /*@null@*/ struct square *target_square = NULL;
    int pointer_index = -1;

    for (int i = 0; i < target_grid->max_y; i++) {
        for (int j = 0; j < target_grid->max_x; j++) {
            // squares[i][j]
            target_square = &target_grid->squares[i][j];

            // save the material in its file
            pointer_index = find_index(target_square->material
                , target_campaign->material_list
                , target_campaign->material_list_len
                , sizeof(struct material));

            check(pointer_index >= -1, "error when calculating"
                " material pointer on square [%d][%d] (pointer is %d)"
                , i, j, pointer_index);

            fprintf(material_file, "%d\n", pointer_index);

            // save every creature, ensuring to seperate using a
                // newline
            for (int creature_index = 0
                ; creature_index < target_square->max_creatures
                ; creature_index++) {
                pointer_index = find_index(
                    target_square->creatures[creature_index]
                    , target_campaign->creature_list
                    , target_campaign->creature_list_len
                    , sizeof(struct creature));

                check(pointer_index >= -1, "error when calculating"
                    " creature pointer on square [%d][%d]"
                    " (creature #%d) (pointer is %d)", i, j
                    , creature_index, pointer_index);

                fprintf(creature_file, "%d\n", pointer_index);
            }
            fprintf(creature_file, "\n");
        }
    }

error:
    return;
}

static void save_inventories(struct campaign *target_campaign
    , FILE *restrict item_file, FILE *restrict creature_file)
{
    /*@null@*/ struct item *target_item = NULL;
    /*@null@*/ struct creature *target_creature = NULL;
    int pointer_index = 0;
    // ----- save item inventories from creatures
        // in the item list
    for (int i = 0; i < target_campaign->item_list_len; i++) {
        target_item = &target_campaign->item_list[i];

        for (int j = 0; j < target_item->inventory_len; j++) {
            pointer_index = find_index(target_item->inventory[j]
                , target_campaign->item_list
                , target_campaign->item_list_len, sizeof(struct item));

            check(pointer_index >= -1, "error when calculating"
                " item pointer on inventory[%d] in item #%d"
                " (pointer is %d)", j, i, pointer_index);

            fprintf(item_file, "%d\n", pointer_index);
        }
        fprintf(item_file, "\n");
    }

    // ----- save creature inventories from creatures
        // in the creature list
        // pretty much the same as the code for an item above
        // i know this isn't ideal
    for (int i = 0; i < target_campaign->creature_list_len; i++) {
        target_creature = &target_campaign->creature_list[i];

        for (int j = 0; j < target_creature->inventory_len; j++) {
            pointer_index = find_index(target_creature->inventory[j]
                , target_campaign->creature_list
                , target_campaign->creature_list_len
                , sizeof(struct creature));

            check(pointer_index >= -1, "error when calculating"
                " creature pointer on inventory[%d] in creature #%d"
                " (pointer is %d)", j, i, pointer_index);

            fprintf(creature_file, "%d\n", pointer_index);
        }
        fprintf(creature_file, "\n");
    }
error:
    return;
}

// NOTE: all loading functions are essentially the same. This is bad
    // practice but I can make it better when I have the time
    // to think about it
static void load_creature_inventory(struct campaign *target_campaign
    , FILE *restrict creature_file)
    // TODO generalize from items to anything else with an inventory
    // counts and resets num_inv_items just in case
{
    char *char_buffer;
    size_t char_buffer_len = 10; // an initial size
    ssize_t nread = -1;
    long index_val = -1; // index sourced from the file

    int inventory_index = 0;
    int creature_list_index = 0;
    /*@null@*/ struct creature *target_creature = NULL;
    int num_inv_items = 0;

    // ----- init char_buffer
    char_buffer = calloc(char_buffer_len, sizeof(char));
    if (char_buffer == NULL)
        return;

    char_buffer = memset(char_buffer, 0, char_buffer_len * sizeof(*char_buffer));

    // ----- read from file
    while (true) {
        target_creature = &target_campaign
            ->creature_list[creature_list_index];
        // ----- read a line from the file
        nread = getline(&char_buffer, &char_buffer_len, creature_file);
        //printf("nread is %d\n", nread);

        // ----- break out of reading when EOF found
        if (nread == -1)
            break;

        if (char_buffer[0] == '\n') {
            target_creature->num_inv_items = num_inv_items;
            num_inv_items = 0;

            inventory_index = 0;
            creature_list_index++;
            continue;
        }

        // --- get index
        index_val = strtol(char_buffer, NULL, 10);
        //printf("index_val is %d, char_buffer is \"%s\"\n", (int) index_val
        //    , char_buffer);

        // --- set the item in the inventory 
        if (index_val == -1) { // set to NULL when its -1
            target_creature->inventory[inventory_index] = NULL;
            //printf("setting creature_list[%d].inventory[%d] to NULL\n"
            //    , creature_list_index, inventory_index);
        } else { // set the pointer to the index from the file
            target_creature->inventory[inventory_index]
                = &target_campaign->item_list[index_val];
            //printf("setting creature_list[%d].inventory[%d] to %p\n"
            //    , creature_list_index, inventory_index
            //    , &target_campaign->material_list[index_val]);
            num_inv_items++;
        }
        inventory_index++;

        // --- check when to move to the next array
        //if (inventory_index >= target_creature->inventory_len) {
            // TODO maybe check, since this should never run
        //}
    }

    free(char_buffer);
}

void load_item_inventory(struct campaign *target_campaign
    , FILE *restrict item_file)
    // TODO generalize from items to anything else with an inventory
    // counts and resets num_inv_items just in case
{
    char *char_buffer;
    size_t char_buffer_len = 10; // an initial size
    ssize_t nread = -1;
    long index_val = -1; // index sourced from the file

    int inventory_index = 0;
    int item_list_index = 0;
    /*@null@*/ struct item *target_item = NULL;
    int num_inv_items = 0;

    // ----- init char_buffer
    char_buffer = calloc(char_buffer_len, sizeof(char));
    if (char_buffer == NULL)
        return;

    char_buffer = memset(char_buffer, 0, char_buffer_len * sizeof(*char_buffer));

    // ----- read from file
    while (true) {
        target_item = &target_campaign->item_list[item_list_index];
        // ----- read a line from the file
        nread = getline(&char_buffer, &char_buffer_len, item_file);
        //printf("nread is %d\n", nread);

        // ----- break out of reading when EOF found
        if (nread == -1)
            break;

        if (char_buffer[0] == '\n') {
            target_item->num_inv_items = num_inv_items;
            num_inv_items = 0;

            inventory_index = 0;
            item_list_index++;
            continue;
        }

        // --- get index
        index_val = strtol(char_buffer, NULL, 10);
        //printf("index_val is %d, char_buffer is \"%s\"\n", (int) index_val
        //    , char_buffer);

        // --- set the item in the inventory 
        if (index_val == -1) { // set to NULL when its -1
            target_item->inventory[inventory_index] = NULL;
            //printf("setting item_list[%d].inventory[%d] to NULL\n"
            //    , item_list_index, inventory_index);
        } else { // set the pointer to the index from the file
            target_item->inventory[inventory_index]
                = &target_campaign->item_list[index_val];
            //printf("setting item_list[%d].inventory[%d] to %p\n"
            //    , item_list_index, inventory_index
            //    , &target_campaign->material_list[index_val]);
            num_inv_items++;
        }
        inventory_index++;

        // --- check when to move to the next array
        //if (inventory_index >= target_item->inventory_len) {
            // TODO maybe check, since this should never run
        //}
    }

    free(char_buffer);
}

void load_grid_material_ptrs(struct campaign *target_campaign
    , FILE *restrict material_file)
    // loads the material and creature values for a grid
    // with updated pointers rather than the old ones

    // ZERO checks for a valid save: this must use a file
    // that was directly generated by save_grid_ptrs
    // TODO include characters
{
    struct grid *target_grid = &target_campaign->encounter_grid;
    char *char_buffer;
    size_t char_buffer_len = 10; // an initial size
    ssize_t nread = -1;
    long index_val = -1;
    struct coord grid_pos = {0, 0}; // position in the grid

    // ----- init char_buffer
    char_buffer = calloc(char_buffer_len, sizeof(char));
    if (char_buffer == NULL)
        return;

    char_buffer = memset(char_buffer, 0, char_buffer_len * sizeof(*char_buffer));

    // ----- read from file
    while (true) {
        // ----- read a line from the file
        nread = getline(&char_buffer, &char_buffer_len, material_file);
        //printf("nread is %d\n", nread);

        // ----- break out of reading when EOF found
        if (nread == -1)
            break;

        // --- get index
        index_val = strtol(char_buffer, NULL, 10);

        // --- set the material
        if (index_val == -1) { // set to NULL when its -1
            target_grid->squares[grid_pos.y][grid_pos.x].material = NULL;
            //printf("setting [%d][%d] to NULL\n", grid_pos.y, grid_pos.x);
        } else { // set to the index of the material_list
            target_grid->squares[grid_pos.y][grid_pos.x].material
                = &target_campaign->material_list[index_val];
            //printf("setting [%d][%d] to %p\n", grid_pos.y, grid_pos.x
            //    , &target_campaign->material_list[index_val]);
        }
        grid_pos.x++;

        // --- check when to move to the next array
        if (grid_pos.x == target_grid->max_x) {
            grid_pos.y++;
            grid_pos.x = 0;
        }
    }

    free(char_buffer);
}

void load_grid_creature_ptrs(struct campaign *target_campaign
    , FILE *restrict material_file)
{
    struct grid *target_grid = &target_campaign->encounter_grid;
    char *char_buffer;
    size_t char_buffer_len = 10; // an initial size
    ssize_t nread = -1;
    long index_val = -1;
    struct coord grid_pos = {0, 0}; // position in the grid
    int creatures_index = 0;

    // ----- init char_buffer
    char_buffer = calloc(char_buffer_len, sizeof(char));
    if (char_buffer == NULL)
        return;

    char_buffer = memset(char_buffer, 0, char_buffer_len * sizeof(*char_buffer));

    // ----- read from file
    while (true) {
        // ----- read a line from the file
        nread = getline(&char_buffer, &char_buffer_len, material_file);
        //printf("nread is %d\n", nread);

        // ----- break out of reading when EOF found
        if (nread == -1)
            break;

        // ----- when an empty line is found (which means to
            // move to the next square in the grid)
        if (char_buffer[0] == '\n') {
            grid_pos.x++;
            creatures_index = 0;

            // --- check when to move to the next array
            if (grid_pos.x == target_grid->max_x) {
                grid_pos.y++;
                grid_pos.x = 0;
            }
            continue;
        }

        // --- get index
        index_val = strtol(char_buffer, NULL, 10);

        // --- set the material
        if (index_val == -1) { // set to NULL when its -1
            //printf("setting [%d][%d] to NULL\n", grid_pos.y, grid_pos.x);
            target_grid->squares[grid_pos.y][grid_pos.x]
              .creatures[creatures_index] = NULL;

        } else { // set to the index of the material_list
            target_grid->squares[grid_pos.y][grid_pos.x]
                .creatures[creatures_index]
                = &target_campaign->creature_list[index_val];

            //printf("setting [%d][%d] to %p\n", grid_pos.y, grid_pos.x
            //    , &target_campaign->creature_list[creatures_index]);
        }
        creatures_index++;
    }

    free(char_buffer);
}

void load_grid_item_ptrs(struct campaign *target_campaign
    , FILE *restrict item_file)
    // TODO bugged maybe
{
    struct grid *target_grid = &target_campaign->encounter_grid;
    char *char_buffer;
    size_t char_buffer_len = 10; // an initial size
    ssize_t nread = -1;
    long index_val = -1;
    struct coord grid_pos = {0, 0}; // position in the grid
    int items_index = -1;

    // ----- init char_buffer
    char_buffer = calloc(char_buffer_len, sizeof(char));
    if (char_buffer == NULL)
        return;

    char_buffer = memset(char_buffer, 0, char_buffer_len * sizeof(*char_buffer));

    // ----- read from file
    while (true) {
        // ----- read a line from the file
        nread = getline(&char_buffer, &char_buffer_len, item_file);
        //printf("nread is %d\n", nread);

        // ----- break out of reading when EOF found
        if (nread == -1)
            break;

        // ----- when an empty line is found (which means to
            // move to the next square in the grid)
        if (char_buffer[0] == '\n') {
            grid_pos.x++;
            items_index = 0;

            // --- check when to move to the next array
            if (grid_pos.x == target_grid->max_x) {
                grid_pos.y++;
                grid_pos.x = 0;
            }
            continue;
        }

        // --- get index
        index_val = strtol(char_buffer, NULL, 10);

        // --- set the material
        if (index_val == -1) { // set to NULL when its -1
            printf("setting [%d][%d] to NULL\n", grid_pos.y, grid_pos.x);
            target_grid->squares[grid_pos.y][grid_pos.x]
              .items[items_index] = NULL;

        } else { // set to the index of the material_list
            target_grid->squares[grid_pos.y][grid_pos.x]
                .items[items_index]
                = &target_campaign->item_list[items_index];

            printf("setting [%d][%d] to %p\n", grid_pos.y, grid_pos.x
                , &target_campaign->item_list[items_index]);
        }
        items_index++;
    }

    free(char_buffer);
}


void save_campaign(struct campaign *target_campaign)
    // TODO finish this -> make paths with set names
    // TODO do dynamic paths later
    // based on the folder path (use malloc to concat strings)
    // then implement into save_inventories
    // saving a campaign will create: bin.save, grid_creature.save
        // grid_material.save, creature_inv.save, item_inv.save
    // will save in save folder relative to the Makefile
{
    // ----- create files
    // inventory files
    FILE *item_inv_file = fopen("save/item_inv.save", "w");
    FILE *creature_inv_file = fopen("save/creature_inv.save", "w");

    // files storing grid position
    FILE *creature_grid_file = fopen("save/grid_creature.save", "w");
    FILE *material_grid_file = fopen("save/grid_material.save", "w");

    // binary
    FILE *binary_file = fopen("save/bin.save", "w");
    
    if (item_inv_file == NULL || creature_inv_file == NULL
        || creature_grid_file == NULL || material_grid_file == NULL
        || binary_file == NULL) {
        return;
    }

    // ----- save the data
    save_grid_ptrs(target_campaign, material_grid_file, creature_grid_file);
    save_inventories(target_campaign, item_inv_file, creature_inv_file);
    (void) fwrite(target_campaign, sizeof(*target_campaign), 1
        , binary_file);

    // ----- close files
    (void) fclose(material_grid_file);
    (void) fclose(creature_grid_file);
    (void) fclose(item_inv_file);
    (void) fclose(creature_inv_file);
    (void) fclose(binary_file);
}

void load_campaign(struct campaign *target_campaign)
{
    // ----- create files
    // inventory files
    FILE *item_inv_file = fopen("save/item_inv.save", "r");
    FILE *creature_inv_file = fopen("save/creature_inv.save", "r");

    // files storing grid position
    FILE *creature_grid_file = fopen("save/grid_creature.save", "r");
    FILE *material_grid_file = fopen("save/grid_material.save", "r");

    // binary
    FILE *binary_file = fopen("save/bin.save", "r");

    if (item_inv_file == NULL || creature_inv_file == NULL
        || creature_grid_file == NULL || material_grid_file == NULL
        || binary_file == NULL) {
        return;
    }
    
    // ----- read from file
    (void) fread(target_campaign, sizeof(*target_campaign), 1
        , binary_file);

    load_grid_material_ptrs(target_campaign, material_grid_file);
    load_grid_creature_ptrs(target_campaign, creature_grid_file);
    load_item_inventory(target_campaign, item_inv_file);
    load_creature_inventory(target_campaign, creature_inv_file);

    // ----- close files
    (void) fclose(material_grid_file);
    (void) fclose(creature_grid_file);
    (void) fclose(item_inv_file);
    (void) fclose(creature_inv_file);
    (void) fclose(binary_file);
}

void mvwdisplay_campaign_info(WINDOW *win, int y, int x
    , struct campaign *target_campaign)
    // displays information about a campaign
{
    (void) werase(win);
    (void) mvwprintw(win, y, x, "Campaign info:");
    (void) mvwprintw(win, y + 1, x, "Name: \"%s\""
        , target_campaign->name);
    (void) mvwprintw(win, y + 2, x, "Notes: \"%s\""
        , target_campaign->note.string);
}
