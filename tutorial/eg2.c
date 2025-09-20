#include <stdlib.h>
#include <ncurses.h> // stdio.h is included

int main(int argc, char *argv[])
{
        int ch;

        initscr(); // enter ncurses
        raw(); // set the user input to immediately send
            // to the program instead of the terminal
            // buffering it until a newline appears
        keypad(stdscr, TRUE); // allows non character keys to be
            // used by the program
        noecho(); // doesn't echo the output of the user
            // to the terminal automatically

        printw("Type any character to see it in bold\n");
        ch = getch(); // gets a character but supports raw?

        if (ch == KEY_F(1)) // if ch = F1 key
            printw("F1 key pressed");
        else {
            printw("The pressed key is ");
            attron(A_BOLD); // enable bold
            printw("%c", ch);
            attroff(A_BOLD); // disable bold
        }
        refresh(); // update the real screen
        getch(); // wait for user input so the user can read the thing
        endwin(); // end window and exit
        return 0;
}
