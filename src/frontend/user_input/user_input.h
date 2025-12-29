#ifndef USER_INPUT
#define USER_INPUT

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <math.h>


// Rückgabewerte der Funktionen
#define VALID_USER_INPUT 0
#define INVALID_USER_INPUT -1
#define BUFFER_ERROR -2
#define INVALID_FUNCTION_INPUT -3
#define MEM_ALLOC_ERROR -4
#define FLUSH_COMPLETE 1

// Eingabeoptionen auf verschiedenen Bildschirmen
#define LIMIT_INPUT_ATTEMPTS 4

#define INPUT_WELC_SCR L"vhq"
#define SIZE_INPUT_WELC_SCR 3

#define INPUT_OVERVIEW_SCR L"adnmbhq"
#define SIZE_INPUT_OVERVIEW_SCR 7

#define INPUT_AVERAGE_SCR L"nmvbhq"
#define SIZE_INPUT_AVERAGE_SCR 6

#define INPUT_HELP_SCR L"vdq"
#define SIZE_INPUT_HELP_SCR 3

#define INPUT_WS_OR_SS L"ws"
#define SIZE_INPUT_WS_OR_SS 2

#define INPUT_YES_OR_NO L"jn"
#define SIZE_INPUT_YES_OR_NO 2

#define ADD_TO_OLD_OR_NEW_MOD L"hn"
#define SIZE_ADD_TO_OLD_OR_NEW_MOD 2



void input_test();
int read_command(wchar_t valid_input[], size_t size);
int read_string(wchar_t **s);
int read_note(double *s);
int read_lp(int *lp);
int read_jahr(int *jahr);
int read_number_in_bound(int lower_bound, int upper_bound, int *number);
int flush();

#endif

