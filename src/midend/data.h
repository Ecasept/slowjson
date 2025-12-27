#ifndef DATA_HEADER
#define DATA_HEADER

struct Modulgruppe {
    char *name;
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
    char *name;
    double note;
    int lp;
    int modulgruppenindex;
    struct Semester semester;
    enum State state;
};



#endif