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
    
    // print information if the program is not working properly
    if (argc != 2) {
        printf("Usage: %s <a c file name>\n", argv[0]);
        return 1;
    }
    
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Cannon open input file");
        return 1;
    }
    initscr();
    getmaxyx(stdscr, row, col);
    while((ch = fgetc(fp)) != EOF) {
        getyx(stdscr, y, x);
        
        // this looks if the cursor is at the penultimate line and stops
            // printing values to print the informational line
        if (y == (row - 1)) {
            printw("<- Press any key ->");
            getch();
            clear(); // clear the screen
            move(0, 0); // move the cursor to the start of the program
        }
        
        if(prev == '/' && ch == '*') {
            attron(A_BOLD);
            getyx(stdscr, y, x);
            move (y, x - 1); // move back a character
            printw("%c%c", '/', ch);
        } else
            addch(ch);
        
        refresh();
        
        if (prev == '*' && ch == '/')
            attroff(A_BOLD);
        
        prev = ch;

    }
    
    endwin();
    fclose(fp);
    return 0;
}
