struct Modulgruppe {
    char *name;
    int lp_todo;
};
#define Modulgruppe struct Modulgruppe

enum Jahreszeit {
    Winter,
    Sommer
};
#define Jahreszeit enum Jahreszeit

// WS 25/26 = 2025 Winter
struct Semester {
    int jahr;
    Jahreszeit jahreszeit;
};
#define Semester struct Semester

enum State {
    Bestanden,
    NichtBestanden, 
    Ausstehend
};    
#define State enum State

struct Veranstaltung {
    char *name;
    double note;
    int lp;
    int modulindex;
    Semester semester;
    State state;
};
#define Veranstaltung struct Veranstaltung
