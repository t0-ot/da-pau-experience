#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "../../include/engine/terminal.h"

struct termios original_term;

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &original_term);
    raw = original_term;

    raw.c_lflag &= ~(ICANON | ECHO);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    printf("\033[?25l");
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_term);
    printf("\033[?25h");
}

char inputHandler() {
    char latest_ch = 0, temp_ch;
    while (read(STDIN_FILENO, &temp_ch, 1) > 0) {
        latest_ch = temp_ch;
    }
    if (latest_ch == 0) return '\0';
    return latest_ch;
}

int menuInputHandler(int *menuChoice, int optionAmount) {
    char input = inputHandler();

    if (input == '\0') return 0;

    switch (input) {
        case 'w':
            if (*menuChoice - 1 >= 0)
                (*menuChoice)--;
            break;
        case 's':
            if (*menuChoice + 1 < optionAmount)
                (*menuChoice)++;
            break;
        case '\n':
            return 1;
    }
    return 0;
}
