#ifndef USER_INPUT
#define USER_INPUT

#include <stdlib.h>
#include <stdio.h>

// Rückgabewerte der Funktionen
#define INVALID_USER_INPUT -1
#define BUFFER_ERROR -2
#define INVALID_FUNCTION_INPUT -3
#define FLUSH_COMPLETE 1

// Eingabeoptionen auf verschiedenen Bildschirmen
#define LIMIT_INPUT_ATTEMPTS 4

#define INPUT_WELC_SCR "vhq"
#define SIZE_INPUT_WELC_SCR 3

#define INPUT_OVERVIEW_SCR "adnmbhq"
#define SIZE_INPUT_OVERVIEW_SCR 7

#define INPUT_AVERAGE_SCR "nmvbhq"
#define SIZE_INPUT_AVERAGE_SCR 6

#define INPUT_HELP_SCR "vdq"
#define SIZE_INPUT_HELP_SCR 3



void input_test();
int read_command(char valid_input[], size_t size);
int flush();

#endif

