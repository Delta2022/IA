// NOTE: this is not tested. TODO that
#include <ncurses.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int ch = 0;
    int prev = EOF;
    int row = 0;
    int col = 0;
    
    FILE *fp;
    
    int x = 0;
    int y = 0;
    
    // ----- print information if the program is not working properly
    if (argc != 2) {
        printf("Usage: %s <a c file name>\n", argv[0]);
            // argv[0] is the command, which may change if the
            // compiled binary changes
        exit(1); // immediately exit the program
            // (at this point returning is fine but exit works
            // better when stuff gets complicated)
    }
    
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Cannot open input file");
        exit(1);
    }
    initscr(); // enter ncurses
    getmaxyx(stdscr, row, col); // get window limits

    // ----- continuously print a character
    while((ch = fgetc(fp)) != EOF) {
        getyx(stdscr, y, x);
        
        // this looks if the cursor is at the penultimate line and stops
            // printing values to print the informational line
        if (y == (row - 1)) {
            attrset(A_NORMAL); // set the attributes to the default
            printw("<- Press any key ->");

            getch();
            clear(); // clear the screen
            move(0, 0); // move the cursor to the start of the program
        }
        
        if (prev == '/' && ch == '*') { // turns text reversed
            // if /* is found
            attron(A_REVERSE);
            getyx(stdscr, y, x);

            // -- override the already printed '/' character
                // with one that has the attribute
            move (y, x - 1); // move back a character
            addstr("/*"); // print the character
        } else
            addch(ch); // actually add the character
        
        refresh(); // update screen
        
        if (prev == '*' && ch == '/')
            attroff(A_REVERSE);
        
        prev = ch;

    }
    attrset(A_NORMAL); // set the attributes to the default
    mvprintw(row - 1, 0, "<- Press any key ->");
    getch();
    
    endwin();
    fclose(fp);
    return 0;
}
