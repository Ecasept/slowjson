#ifndef DATA_HEADER
#define DATA_HEADER

#include <wchar.h>

struct Modulgruppe {
    wchar_t *name;
    int lp_todo;
    int modulgruppenindex;
};



enum Jahreszeit {
    Winter,
    Sommer
};


// WS 25/26 = 2025 Winter
struct Semester {
    int jahr;
    enum Jahreszeit jahreszeit;
};


enum State {
    Bestanden,
    NichtBestanden, 
    Ausstehend
};    


struct Veranstaltung {
    wchar_t *name;
    double note;
    int lp;
    int modulgruppenindex;
    struct Semester semester;
    enum State state;
};



#endif