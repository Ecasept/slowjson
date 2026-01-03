#ifndef TEXT_FORMATTING
#define TEXT_FORMATTING

#include <stdio.h>
#include "../../midend/data.h"
#include <wchar.h>

// Textfarben
#define TXT_RED L"\033[31m"
#define TXT_GREEN L"\033[32m"
#define TXT_YELLOW L"\033[33m"
#define TXT_BLUE L"\033[34m"
#define TXT_MAGENTA L"\033[35m"
#define TXT_CYAN L"\033[36m"
#define TXT_WHITE L"\033[37m"

// Hintergrundfarben
#define BACK_RED L"\033[41m"
#define BACK_GREEN L"\033[42m"
#define BACK_YELLOW L"\033[43m"
#define BACK_BLUE L"\033[44m"
#define BACK_MAGENTA L"\033[45m"
#define BACK_CYAN L"\033[46m"
#define BACK_WHITE L"\033[47m"

// Text Styles
#define TXT_BOLD L"\033[1m"
#define TXT_DIM L"\033[2m"
#define TXT_ITALIC L"\033[3m"
#define TXT_UNDERLINED L"\033[4m"
#define TXT_INVERSE L"\033[7m"

#define CURSOR_INVISIBLE L"\033[8m"

// Text Farben/Styles zurücksetzen
#define END_STYLE L"\033[0m"


// Sortierung Veranstaltungsübersicht
#define SORT_TIME 1
#define SORT_MODUL 2


// Codes für verschiedene Bildschirme
#define CURR_PAGE_OV_SCR 1
#define CURR_PAGE_AV_SCR 2
#define CURR_PAGE_HELP_SCR 3

// view_type der Veranstaltungsüberischt
#define OV_BY_TIME 1
#define OV_BY_MOD 2

#define NEW_ENTRY 1
#define NO_NEW_ENTRY 0


// PO für Notendurchschnitt
#define PO_2018 L"FPO Bachelor Informatik 2018"


// Anzeige der Veranstaltungen
#define BESTANDEN L"bestanden"
#define NICHT_BESTANDEN L"nicht bestanden"
#define AUSSTEHEND L"ausstehend"
#define STATUS L"Status"
#define NOTE L"Note"
#define LP L"LP"


void print_welcomescreen(void);
void print_welcomescreen_options(void);
int print_overviewscreen(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod, int view_type, int new_entry);
void print_helpscreen(void);
void print_endscreen(void);
int print_averagescreen(int new_entry, struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod);
void clear_display(void);
void print_overview_by_time(struct Veranstaltung *ver, size_t size_ver);
void print_overview_by_mod(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod);
int print_addverscreen(struct Veranstaltung **ver, size_t *size_ver, struct Modulgruppe **mod, size_t *size_mod);
void print_inputcompletescreen(void);
int print_addmodscreen(struct Modulgruppe **mod, size_t *size_mod);
void print_buffer_error_screen(void);
void print_memalloc_error_screen(void);
void print_wrong_command_screen(void);
void print_wrong_string_screen(void);
void print_wrong_function_input_screen(void);
void print_loaddata_error_screen(wchar_t *error_message);
void print_loaddata_nofile_screen(wchar_t *error_message);
void print_loaddata_complete_screen(void);
void print_newsavefile_created(void);
void print_no_ver_saved_screen(void);
void print_savedata_complete_screen(void);
void print_savedata_error_screen(wchar_t *error_message);

void free_all(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod);



#endif
