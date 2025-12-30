#ifndef MID_H
#define MID_H

#include "data.h"
#include <stddef.h>

double notendurchschnitt_dumm(struct Veranstaltung *ver, size_t size_ver);
double notendurchschnitt_po(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod);
void sort_by_time(struct Veranstaltung *ver, size_t size_ver);
void sort_by_mod(struct Veranstaltung *ver, size_t size_ver);

#endif