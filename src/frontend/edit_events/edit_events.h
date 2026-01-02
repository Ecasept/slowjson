#ifndef EDIT_EVENTS
#define EDIT_EVENTS

#include "../../midend/data.h"
#include <stdio.h>
#include <wchar.h>

#define STANDARD_SPACE(a) {for (int i = 0; i < 20 - (int) a; ++i) {wprintf(L" ");}}


int print_editver(struct Veranstaltung **ver, size_t *size_ver, struct Modulgruppe **mod, size_t *size_mod, int *current_page);
int select_ver(struct Veranstaltung **ver, size_t *size_ver, int *selected_ver);
void print_selected_ver(struct Veranstaltung *ver, struct Modulgruppe *mod, size_t size_mod, int selected_ver);
void print_options_editver();
int delete_verscreen(struct Veranstaltung **ver, size_t *size_ver, struct Modulgruppe **mod, size_t *size_mod, int *selected_ver);
void print_eventdeleted();
int print_noverscreen();
int change_notescreen(struct Veranstaltung **ver, struct Modulgruppe **mod, size_t *size_mod, int selected_ver);
void print_notechangedscreen();
int change_modscreen(struct Veranstaltung **ver, struct Modulgruppe **mod, size_t *size_mod, int selected_ver);
void print_modchangedscreen();


#endif
