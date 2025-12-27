#ifndef TEXT_FORMATTING
#define TEXT_FORMATTING

#include <stdio.h>
#include "../../midend/data.h"

// Textfarben
#define TXT_RED "\033[31m"
#define TXT_GREEN "\033[32m"
#define TXT_YELLOW "\033[33m"
#define TXT_BLUE "\033[34m"
#define TXT_MAGENTA "\033[35m"
#define TXT_CYAN "\033[36m"
#define TXT_WHITE "\033[37m"

// Hintergrundfarben
#define BACK_RED "\033[41m"
#define BACK_GREEN "\033[42m"
#define BACK_YELLOW "\033[43m"
#define BACK_BLUE "\033[44m"
#define BACK_MAGENTA "\033[45m"
#define BACK_CYAN "\033[46m"
#define BACK_WHITE "\033[47m"

// Text Styles
#define TXT_BOLD "\033[1m"
#define TXT_DIM "\033[2m"
#define TXT_ITALIC "\033[3m"
#define TXT_UNDERLINED "\033[4m"
#define TXT_INVERSE "\033[7m"

// Text Farben/Styles zurücksetzen
#define END_STYLE "\033[0m"


// Sortierung Veranstaltungsübersicht
#define SORT_TIME 1
#define SORT_MODUL 2


void print_welcomescreen();
int print_overviewscreen(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod, int view_type);
void print_helpscreen();
void print_endscreen();
int print_averagescreen();
void clear_display();
void print_overview_by_time(struct Veranstaltung *ver, size_t size_ver);



#endif