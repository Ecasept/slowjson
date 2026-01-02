#include "data.h"
#include <stdlib.h>
#include <wchar.h>

double notendurchschnitt_dumm(struct Veranstaltung *ver, size_t size_ver)
{
    double sum_grade_lp = 0.0;
    int sum_lp = 0;

    for (size_t i = 0; i < size_ver; ++i) {
        if (ver[i].state == Bestanden) {
            sum_grade_lp += ver[i].note * ver[i].lp;
            sum_lp += ver[i].lp;
        }
    }

    if (sum_lp == 0) {
        return 0.0;
    }

    return sum_grade_lp / sum_lp;
}

double notendurchschnitt_po(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod)
{    
    return 1.0;
}

void swap(struct Veranstaltung *a, struct Veranstaltung *b)
{
    struct Veranstaltung temp = *a;
    *a = *b;
    *b = temp;
}

int partition_by_time(struct Veranstaltung *ver, int low, int high)
{
    struct Veranstaltung pivot = ver[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (ver[j].semester.jahr < pivot.semester.jahr) {
            i++;
            swap(&ver[i], &ver[j]);
        } else if (ver[j].semester.jahr == pivot.semester.jahr) {
            if (ver[j].semester.jahreszeit > pivot.semester.jahreszeit) {
                i++;
                swap(&ver[i], &ver[j]);
            } else if (ver[j].semester.jahreszeit == pivot.semester.jahreszeit) {
                if (wcscmp(ver[j].name, pivot.name) < 0) {
                    i++;
                    swap(&ver[i], &ver[j]);
                }
            }
        }
    }

    swap(&ver[i + 1], &ver[high]);
    return i + 1;
}

void quicksort_by_time(struct Veranstaltung *ver, int low, int high)
{
    if (low < high) {
        int p = partition_by_time(ver, low, high);

        quicksort_by_time(ver, low, p - 1);
        quicksort_by_time(ver, p + 1, high);
    }
}

void sort_by_time(struct Veranstaltung *ver, size_t size_ver)
{
    if (size_ver == 0)
        return;
    quicksort_by_time(ver, 0, size_ver - 1);
}

int partition_by_mod(struct Veranstaltung *ver, int low, int high)
{
    struct Veranstaltung pivot = ver[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (ver[j].modulgruppenindex < pivot.modulgruppenindex) {
            i++;
            swap(&ver[i], &ver[j]);
        } else if (ver[j].modulgruppenindex == pivot.modulgruppenindex) {
            if (wcscmp(ver[j].name, pivot.name) < 0) {
                i++;
                swap(&ver[i], &ver[j]);
            }
        }
    }

    swap(&ver[i + 1], &ver[high]);
    return i + 1;
}

void quicksort_by_mod(struct Veranstaltung *ver, int low, int high)
{
    if (low < high) {
        int p = partition_by_mod(ver, low, high);

        quicksort_by_mod(ver, low, p - 1);
        quicksort_by_mod(ver, p + 1, high);
    }
}

void sort_by_mod(struct Veranstaltung *ver, size_t size_ver)
{
    if (size_ver == 0)
        return;
    quicksort_by_mod(ver, 0, size_ver - 1);
}

// sort mod by alpha

void swap_mod(struct Modulgruppe *a, struct Modulgruppe *b)
{
    struct Modulgruppe temp = *a;
    *a = *b;
    *b = temp;
}

int partition_mod_by_alpha(struct Modulgruppe *mod, int low, int high)
{
    struct Modulgruppe pivot = mod[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {

            if (wcscmp(mod[j].name, pivot.name) < 0) {
                i++;
                swap_mod(&mod[i], &mod[j]);
            }
        
    }

    swap_mod(&mod[i + 1], &mod[high]);
    return i + 1;
}

void quicksort_mod_by_alpha(struct Modulgruppe *mod, int low, int high)
{
    if (low < high) {
        int p = partition_mod_by_alpha(mod, low, high);

        quicksort_mod_by_alpha(mod, low, p - 1);
        quicksort_mod_by_alpha(mod, p + 1, high);
    }
}

void sort_mod_by_alpha(struct Modulgruppe *mod, size_t size_mod)
{
    if (size_mod == 0)
        return;
    quicksort_mod_by_alpha(mod, 0, size_mod - 1);
}