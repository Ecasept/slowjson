#pragma once

#include <stdio.h>
#include "../midend/data.h"

#define SIZE_VER 7
#define SIZE_MOD 4
#define SUCCESS 1

void printer_test(void);
struct Veranstaltung *get_test_ver(void);
struct Modulgruppe *get_test_mod(void);
int get_test_data(struct Veranstaltung **ver, size_t *size_ver, struct Modulgruppe **mod, size_t *size_mod);
