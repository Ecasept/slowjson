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
#define CHANGES_SAVED -5
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

#define INPUT_EDIT_VER L"lnmvch"
#define SIZE_INPUT_EDIT_VER 6

#define INPUT_BACK_TO_OV L"z"
#define SIZE_INPUT_BACK_TO_OV 1

#define INPUT_NOTE_OR_AUSSTEHEND L"nab"
#define SIZE_INPUT_NOTE_OR_AUSSTEHEND 3


#define INPUT_MOD_OR_CANCEL L"nb"
#define SIZE_INPUT_MOD_OR_CANCEL 2


// Fehlerbehandlung
// Weiterleitung der vier Fehlerfälle 1. BUFFER_ERROR 2. INVALID_FUNCTION_INPUT 3. MEM_ALLOC_ERROR 4. INVALID_USER_INPUT an die aufrufende Funktion
#define STANDARD_ERROR_HANDLING(a) {switch (a) {case BUFFER_ERROR: return BUFFER_ERROR; case INVALID_FUNCTION_INPUT: return INVALID_FUNCTION_INPUT; case INVALID_USER_INPUT: return INVALID_USER_INPUT; case MEM_ALLOC_ERROR: return MEM_ALLOC_ERROR; }}





int read_command(wchar_t valid_input[], size_t size);
int read_string(wchar_t **s);
int read_note(double *s);
int read_lp(int *lp);
int read_jahr(int *jahr);
int read_number_in_bound(int lower_bound, int upper_bound, int *number);
int flush(void);

#endif

