#include <stdio.h>
#include "engine/terminal.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>

void set_raw_mode(int enable) {
    static DWORD original_mode;
    static int is_raw_enabled = 0;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    if (enable) {
        if (is_raw_enabled) return;

        GetConsoleMode(hStdin, &original_mode);
        // Disable line input, echo, and processed input for raw mode
        DWORD raw_mode = original_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
        SetConsoleMode(hStdin, raw_mode);

        // Hide cursor on Windows
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hStdout, &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hStdout, &cursorInfo);

        is_raw_enabled = 1;
    } else {
        if (!is_raw_enabled) return;

        SetConsoleMode(hStdin, original_mode);

        // Restore cursor on Windows
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hStdout, &cursorInfo);
        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(hStdout, &cursorInfo);

        is_raw_enabled = 0;
    }
}

char inputHandler() {
    char latest_ch = 0;
    // _kbhit checks if a key was pressed, _getch reads it instantly non-blocking
    while (_kbhit()) {
        latest_ch = _getch();
    }
    if (latest_ch == 0) return '\0';
    return latest_ch;
}

#else
#include <termios.h>
#include <unistd.h>

void set_raw_mode(int enable) {
    static struct termios original_term;
    static int is_raw_enabled = 0;

    if (enable) {
        if (is_raw_enabled) return;

        struct termios raw;
        tcgetattr(STDIN_FILENO, &original_term);
        raw = original_term;

        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;

        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        printf("\033[?25l"); // Hide cursor (ANSI)
        fflush(stdout);

        is_raw_enabled = 1;
    } else {
        if (!is_raw_enabled) return;

        tcsetattr(STDIN_FILENO, TCSANOW, &original_term);
        printf("\033[?25h"); // Show cursor (ANSI)
        fflush(stdout);

        is_raw_enabled = 0;
    }
}

char inputHandler() {
    char latest_ch = 0, temp_ch;
    while (read(STDIN_FILENO, &temp_ch, 1) > 0) {
        latest_ch = temp_ch;
    }
    if (latest_ch == 0) return '\0';
    return latest_ch;
}
#endif

// The menu input handler logic remains 100% identical for both platforms!
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
        case '\r':
            return 1;
    }
    return 0;
}
