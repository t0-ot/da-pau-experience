#ifndef TERMINAL_H
#define TERMINAL_H

void set_raw_mode(int enable);
char inputHandler();
int menuInputHandler(int *menuChoice, int optionAmount);

#endif // TERMINAL_H
