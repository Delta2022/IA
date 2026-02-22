#include "../include/main.h"

int get_multi_input(WINDOW *win, char **dest, int num_dest
    , int *buffer_max_lens, int *text_pos)
// TODO make it support other windows
// TODO support jumping to jumps in the text
// TODO add wrapping
// TODO allow vertical traverse of text
// TODO rename cur_buffer to be different from current_buffer
    // dest is an array of pointers to the destinations to write to
    // num_dest is the length of dest 
        //(the number of destinations there are)
    // buffer_max_lens is the maximum number of characters to write 
        // to the buffer. its length is num_dest
    // text_pos is the position of where to show the inputted text
        //(this should be an empty line). Its length is num_dest
{
    // ----- simple checks
    if (text_pos == NULL || dest == NULL || *dest == NULL
        || buffer_max_lens == NULL) {
        return -1;
    } else if (num_dest <= 0) {
        return -1;
    }

    // ----- find the maximum buffer size to allocate
    int max_buffer_len = 0;
    for (int i = 0; i < num_dest; i++) {
        if (buffer_max_lens[i] > max_buffer_len) {
            max_buffer_len = buffer_max_lens[i];
        }
    }

    // ----- inits
    int c = 0;
    char c_char = '\0';

    char buffers[num_dest][max_buffer_len]; // stores the texts 
        // inputted by the user
    int buffer_indices[num_dest]; // stores the next clear 
        //position to write a character for each buffer
    int buffer_cursors[num_dest]; // stores the    
        // cursor position for each buffer

    char *current_buffer;
    int *cur_buffer_index;
    int *cur_buffer_cursor;

    // NOTE: each buffer_index and buffer_cursor is parallel to
        // each char array in buffers

    int num_text = num_dest; // stores the number of input fields (texts)
    int cur_buffer = 0; // points to the current buffer 
        // that is being edited in buffers

    struct coord cursor = {0, 0};
    char temp_saves[2] = {'\0', '\0'}; // only used when
        // adding text when "we're not at the end"
        // as a temporary space for characters

    // ----- zero out the arrays
    (void) memset(buffers, 0, num_dest * sizeof(buffers[0]));
    (void) memset(buffer_indices, 0, sizeof(buffer_indices));
    (void) memset(buffer_cursors, 0, sizeof(buffer_cursors));

    // ----- set up the cursor in its correct position 
        // and set cursor.y and cursor.x
    (void) wmove(win, text_pos[0], 0);
    cursor.y = getcury(win);
    cursor.x = getcurx(win);

    // ----- set current values
    current_buffer = buffers[cur_buffer];
    cur_buffer_index = &buffer_indices[cur_buffer];
    cur_buffer_cursor = &buffer_cursors[cur_buffer];

    (void) wrefresh(win);

    // ----- input loop
    while (true) {
        c = wgetch(win);
        // ----- exit condition
        if (c == KEY_F(2)) {
            break;
        }

        switch (c) {
            case 263: // ----- delete
                // dont delete if it can't be deleted
                    // buffer_cursor should always be less than or equal
                    // to buffer_indices so buffer_indices isn't checked
                if (cursor.x <= 0 && *cur_buffer_cursor <= 0) {
                    break;
                }

                // ----- if we aren't at the end
                if (*cur_buffer_cursor != *cur_buffer_index) { 

                    // --- change buffers
                    // starts at the char before cursor pos and shifts
                        // all other chars right by 1
                        // (used the char before because thats how 
                        // editors work)
                    for (int i = *cur_buffer_cursor - 1
                        ; i < *cur_buffer_index; i++) {
                        current_buffer[i] = current_buffer[i + 1];
                    }

                    // --- update values to reflect one character
                        // deleted
                    (*cur_buffer_cursor)--;
                    (*cur_buffer_index)--;
                    cursor.x--;

                    // --- update the screen NOTE: inefficient
                    (void) wmove(win, cursor.y, 0);
                    (void) wclrtoeol(win);
                    // set the last empty char as \0 for printing
                    current_buffer[*cur_buffer_index] = '\0';
                    (void) wprintw(win, "%s", current_buffer);
                    
                    // reset ncurses cursor position to the cursor
                    (void) wmove(win, cursor.y, cursor.x);
                    break;
                }
                
                // ----- if we are at the end
                (void) wmove(win, cursor.y, --cursor.x);
                (void) wdelch(win);

                (*cur_buffer_index)--; // delete a char 
                    // from temp like an hdd deletes data (by just
                    // signalling that it can be overwritten)
                (*cur_buffer_cursor)--;
                break;
            
            // ----- dealing with keys
            case KEY_DOWN:
                if (cur_buffer < num_text - 1) { // the maximum that
                    // cur_buffer can be is num_text - 1 (the max index)
                    cur_buffer++; // change the current buffer to edit
                    
                    // update current values
                    current_buffer = buffers[cur_buffer];
                    cur_buffer_index = &buffer_indices[cur_buffer];
                    cur_buffer_cursor = &buffer_cursors[cur_buffer];

                    // update cursor.y and cursor.x to reflect the 
                        // new current buffer
                    cursor.y = text_pos[cur_buffer];
                    cursor.x = *cur_buffer_cursor;
                    (void) wmove(win, cursor.y, cursor.x);
                }
                break;

            case KEY_UP:
                if (cur_buffer > 0) {
                    cur_buffer--; // change the current buffer to edit

                    // update the current values
                    current_buffer = buffers[cur_buffer];
                    cur_buffer_index = &buffer_indices[cur_buffer];
                    cur_buffer_cursor = &buffer_cursors[cur_buffer];

                    // update cursor.y and cursor.x to reflect 
                        // the new current buffer
                    cursor.y = text_pos[cur_buffer];
                    cursor.x = *cur_buffer_cursor;
                    (void) wmove(win, cursor.y, cursor.x);
                }
                break;

            case KEY_RIGHT:
                if (*cur_buffer_cursor < *cur_buffer_index) {
                    (*cur_buffer_cursor)++;

                    cursor.y = text_pos[cur_buffer];
                    cursor.x = *cur_buffer_cursor;
                    (void) wmove(win, cursor.y, cursor.x);
                }
                break;
                
            case KEY_LEFT:
                if (*cur_buffer_cursor > 0) {
                    (*cur_buffer_cursor)--;

                    cursor.y = text_pos[cur_buffer];
                    cursor.x = *cur_buffer_cursor;
                    (void) wmove(win, cursor.y, cursor.x);
                }
                break;

            default: // ----- typing regular characters
                c_char = (char) c;

                // does not add any characters if the buffer's length
                    // is reached
                if (*cur_buffer_index 
                    == buffer_max_lens[cur_buffer] - 1) {
                    // if the index points to the last empty character
                        // (since max_buffer_len is the max index)
                    break;
                }

                // remove all characters that aren't allowed 
                    // (alphabetical only for now) TODO change
                if (c_char != ' ' 
                    && (c_char < 'a' || c_char > 'z') 
                    && (c_char < 'A' || c_char > 'Z'))
                    break;

                // ----- if we aren't at the end
                if (*cur_buffer_cursor != *cur_buffer_index) {

                    // shift every character left of the buffer left
                        // by one
                    temp_saves[0] = current_buffer[*cur_buffer_cursor];
                    for (int i = *cur_buffer_cursor
                        ; i < *cur_buffer_index; i++) {
                        temp_saves[1] = current_buffer[i + 1];

                        current_buffer[i + 1] = temp_saves[0];

                        temp_saves[0] = temp_saves[1];
                    }
                                        
                    // place the character at the cursor
                        // (instead of the index when we aren't at the
                        // end)
                    current_buffer[*cur_buffer_cursor] = c_char;

                    // update values
                    (*cur_buffer_cursor)++;
                    (*cur_buffer_index)++;
                    cursor.x++;

                    // reprint on the line
                    current_buffer[*cur_buffer_index] = '\0';
                    (void) wmove(win, cursor.y, 0);
                    (void) wclrtoeol(win);
                    (void) wprintw(win, "%s", current_buffer);
                    (void) wmove(win, cursor.y, cursor.x);
                    break;
                }

                // ----- if we are at the end
                // add to window
                (void) waddch(win, (chtype) c_char);

                // set the current buffer's empty position to the
                    // character and update the buffer's cursor
                current_buffer[(*cur_buffer_index)++] = c_char;

                // update the cursor and the buffer_cursor
                cursor.x++;
                (*cur_buffer_cursor)++;
        }

        // ----- debug
        //(void) mvprintw(LINES - 1, 0, "%d", *cur_buffer_cursor);
        //(void) mvprintw(LINES - 2, 0, "%d"
        //  , buffer_max_lens[cur_buffer]);
        //(void) move(cursor.y, cursor.x);
        //(void) refresh();

        // ----- refresh screen (maybe not needed?)
        //wrefresh(win);
    }

    // ----- null terminate string and save it to dest
    for (int i = 0; i < num_text; i++) {
        buffers[i][buffer_indices[i]] = '\0'; // end the string 
            // with a \0, avoiding any 'deleted'
            // (but still present) data from messing things up and
            // ensuring that the string is null terminated

        // copy to dest (safely)
        (void) strncpy(dest[i], buffers[i]
            , (size_t)buffer_max_lens[i]);
        dest[i][buffer_max_lens[i] - 1] = '\0';

        //(void) mvprintw(i + 10, 0, "%s", buffers[i]);
    }

    return 0;
}
