#include "text_formatting.h"
#include "../user_input/user_input.h"
#include "../../midend/data.h"
#include "../../midend/mid.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/ioctl.h>


void print_welcomescreen(void)
{
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                                                                                                                      │\n");
        wprintf(L"│%ls              ████████████████████████████████████████████████████████████████████████████████████████████%ls            │\n", TXT_GREEN, END_STYLE);
        wprintf(L"│%ls              ███ █████ ██ ██ █████ █████ ██ ██       ██  ██████  ██  ██████  ██     ██  █████ ████  █████%ls            │\n", TXT_GREEN, END_STYLE);
        wprintf(L"│%ls              ███ █████ ██ ██ █████ █████ █ ███ █████ ██ █ ████ █ ██ █ ████ █ ██ ██████ █ ████ ████  █████%ls            │\n", TXT_GREEN, END_STYLE);
        wprintf(L"│%ls              ███ █████ ██ ██ █████ █████  ████ █████ ██ ██ ██ ██ ██ ██ ██ ██ ██    ███ ██ ███ ████  █████%ls            │\n", TXT_GREEN, END_STYLE);
        wprintf(L"│%ls              ███ ██ ██ ██ ██ █████ █████  ████ █████ ██ ███  ███ ██ ███  ███ ██    ███ ███ ██ ████  █████%ls            │\n", TXT_GREEN, END_STYLE);
        wprintf(L"│%ls              ███ █ █ █ ██ ██ █████ █████ █ ███ █████ ██ ████████ ██ ████████ ██ ██████ ████ █ ███████████%ls            │\n", TXT_GREEN, END_STYLE);
        wprintf(L"│%ls              ███  ███  ██ ██    ██    ██ ██ ██       ██ ████████ ██ ████████ ██     ██ █████  ████  █████%ls            │\n", TXT_GREEN, END_STYLE);
        wprintf(L"│%ls              ████████████████████████████████████████████████████████████████████████████████████████████%ls            │\n", TXT_GREEN, END_STYLE);
        wprintf(L"│                                                                                                                      │\n");
        wprintf(L"│                                   %lsDies ist ihr persönlicher Studienplaner%ls                                            │\n", TXT_INVERSE, END_STYLE);
        wprintf(L"│                                                                                                                      │\n");
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n");        
}


void print_welcomescreen_options(void)
{
        wprintf(L"\n  %lsOptionen%ls                        %lsTaste%ls                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        wprintf(L"  Veranstaltungsübersicht           [v]                    \n");
        wprintf(L"  Hilfe                             [h]                    \n");
        wprintf(L"  Programm beenden                  [q]                    \n");
        wprintf(L"\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
}









int print_overviewscreen(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod, int view_type, int new_entry)
{


        clear_display();
        if (new_entry) {
                print_inputcompletescreen();      
        }


        wprintf(L"%ls Veranstaltungsübersicht %ls\n", TXT_INVERSE, END_STYLE);
        

        // Übersicht nach Semester geordnet ausgeben
        if (view_type == OV_BY_TIME) {
                print_overview_by_time(ver, size_ver);
        } else if (view_type == OV_BY_MOD) {
                print_overview_by_mod(ver, size_ver, mod, size_mod);
        }


        

        


        wprintf(L"\n\n\n  %lsOptionen%ls                        %lsTaste%ls                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        wprintf(L"  Ansicht ändern                    [a]                    Veranstaltung hinzufügen          [n]\n");
        wprintf(L"  (Sortierung nach Modulgruppen)                           Modulgruppe hinzufügen            [m]\n");
        wprintf(L"  Notendurchschnitt anzeigen        [d]                    Veranstaltung bearbeiten          [b]\n");
        wprintf(L"\n");
        wprintf(L"  Hilfe                             [h]                    \n");
        wprintf(L"  Programm beenden                  [q]                    \n");
        wprintf(L"\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        
        return 0;
}

void print_overview_by_time(struct Veranstaltung *ver, size_t size_ver)
{
        struct Semester last_time;
        last_time.jahr = -1;
        last_time.jahreszeit = -1;

               

        if (size_ver == 0) {
                // Hinweis, dass noch keine Veranstaltung hinzugefügt wurde
                print_no_ver_saved_screen();
        } else {
                // Sortierung der Veranstaltungen nach Semester
                sort_by_time(ver, size_ver);
                wprintf(L"\nSortierung: Semester, aufsteigend\n");
                for (size_t i = 0; i < size_ver; ++i) {

                
                        if (ver[i].semester.jahr != last_time.jahr || ver[i].semester.jahreszeit != last_time.jahreszeit) {
                                struct winsize w;
                                if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
                                        perror("ioctl");
                                }
                                if (ver[i].semester.jahreszeit == Winter) {
                                        wprintf(L"\n\n%ls WS %i/%i %ls\n", TXT_INVERSE, ver[i].semester.jahr, ver[i].semester.jahr + 1, END_STYLE);
                                        for (int a = 0; a < w.ws_col; ++a) {
                                                wprintf(L"─");
                                        }
                                } else {
                                        wprintf(L"\n\n%ls SS %i %ls\n", TXT_INVERSE, ver[i].semester.jahr, END_STYLE);
                                        for (int a = 0; a < w.ws_col; ++a) {
                                                wprintf(L"─");
                                        }                        
                                }

                                last_time.jahr = ver[i].semester.jahr;
                                last_time.jahreszeit = ver[i].semester.jahreszeit;

                                // Bedeutung der Spalten ausgeben    
                                wprintf(L"%lsName der Veranstaltung%ls", TXT_UNDERLINED, END_STYLE);
                                int counter = 22;
                                counter = 90 - counter;
                                for (int a = 0; a < counter; ++a) {
                                        wprintf(L" ");
                                }
                                wprintf(L"                 %ls%ls%ls     %ls%ls%ls   %ls%ls%ls\n", TXT_UNDERLINED, STATUS, END_STYLE, TXT_UNDERLINED, LP, END_STYLE, TXT_UNDERLINED, NOTE, END_STYLE);
                        }

                        // Veranstaltungsnamen ausgeben   
                        wprintf(L"%ls", ver[i].name);
                        int counter = 0;
                        while(ver[i].name[counter] != L'\0') {
                                ++counter;
                        }
                        counter = 90 - counter;
                        for (int a = 0; a < counter; ++a) {
                                wprintf(L" ");
                        }

                        // Restliche Daten der Veranstaltung ausgeben
                        switch(ver[i].state) {
                                case Bestanden:
                                        wprintf(L"        %ls%15ls%ls     %i    %.1f\n", TXT_GREEN, BESTANDEN, END_STYLE, ver[i].lp, ver[i].note);
                                       break;
                                case NichtBestanden:
                                       wprintf(L"        %ls%15ls%ls     %i    %ls%.1f%ls\n", TXT_RED, NICHT_BESTANDEN, END_STYLE, ver[i].lp, TXT_RED, ver[i].note, END_STYLE);
                                        break;
                                case Ausstehend:
                                        wprintf(L"        %ls%15ls%ls     %i    %ls/%ls\n", TXT_YELLOW, AUSSTEHEND, END_STYLE, ver[i].lp, TXT_YELLOW, END_STYLE);
                                        break;
                        }
                }
        }
}

void print_overview_by_mod(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod)
{
        int last_index = -1;
        struct winsize w;
        int sum = -1;
        size_t a = 0;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
                perror("ioctl");
        }

        if (size_ver == 0) {
                // Hinweis, dass noch keine Veranstaltung hinzugefügt wurde
                print_no_ver_saved_screen();
        } else {
        
                // Sortierung der Veranstaltungen nach Modulgruppe
                sort_by_mod(ver, size_ver);

                wprintf(L"\nSortierung: Modulgruppe, alphabetisch\n");


                for (size_t i = 0; i < size_ver; ++i) {


                        if (ver[i].modulgruppenindex != last_index) {

                                if (last_index != -1) {
                                        wprintf(L"\n");
                                        wprintf(L"Summe: %i/%i\n", sum, mod[a].lp_todo);
                                }

                                a = 0;
                                while (a < size_mod) {
                                        if (ver[i].modulgruppenindex == mod[a].modulgruppenindex) {
                                                break;
                                        }
                                        ++a;
                                }
                        
                                wprintf(L"\n\n%ls %ls %ls\n", TXT_INVERSE, mod[a].name, END_STYLE);
                                for (int a = 0; a < w.ws_col; ++a) {
                                                wprintf(L"─");
                                }                        
                                last_index = ver[i].modulgruppenindex;

                                // Bedeutung der Spalten ausgeben    
                                wprintf(L"%lsName der Veranstaltung%ls", TXT_UNDERLINED, END_STYLE);
                                for (int a = 0; a < 90 - 22; ++a) {
                                        wprintf(L" ");
                                }
                                wprintf(L"                 %ls%ls%ls     %ls%ls%ls   %ls%ls%ls\n", TXT_UNDERLINED, STATUS, END_STYLE, TXT_UNDERLINED, LP, END_STYLE, TXT_UNDERLINED, NOTE, END_STYLE);
                                sum = 0;
                        }

                

                        // Veranstaltungsnamen ausgeben   
                        wprintf(L"%ls", ver[i].name);
                        int counter = 0;
                        while(ver[i].name[counter] != L'\0') {
                                ++counter;
                        }
                        counter = 90 - counter;
                        for (int a = 0; a < counter; ++a) {
                                wprintf(L" ");
                        }

                        // Restliche Daten der Veranstaltung ausgeben
                        switch(ver[i].state) {
                                case Bestanden:
                                        wprintf(L"        %ls%15ls%ls     %i    %.1f\n", TXT_GREEN, BESTANDEN, END_STYLE, ver[i].lp, ver[i].note);
                                        break;
                                case NichtBestanden:
                                        wprintf(L"        %ls%15ls%ls     %i    %ls%.1f%ls\n", TXT_RED, NICHT_BESTANDEN, END_STYLE, ver[i].lp, TXT_RED, ver[i].note, END_STYLE);
                                        break;
                                case Ausstehend:
                                        wprintf(L"        %ls%15ls%ls     %i    %ls/%ls\n", TXT_YELLOW, AUSSTEHEND, END_STYLE, ver[i].lp, TXT_YELLOW, END_STYLE);
                                        break;
                        }
                        if (ver[i].state == Bestanden) {
                                sum += ver[i].lp;
                        }
                }

        
                for (int b = 0; b < 90; ++b) {
                        wprintf(L" ");
                }
                wprintf(L"\nSumme: %i/%i\n", sum, mod[a].lp_todo);
        }
                        



}









void print_helpscreen(void)
{
        clear_display();


        wprintf(L"%ls Hilfe %ls\n\n", TXT_INVERSE, END_STYLE);

        wprintf(L"\n%lsAllgemeines%ls\n", TXT_UNDERLINED, END_STYLE);
        wprintf(L"- An jedem Seitenende werden die aktuell verfügbaren \"%lsOptionen%ls\" angezeigt.\n", TXT_UNDERLINED, END_STYLE);
        wprintf(L"- Auf jeder Seite werden nur die jeweils angezeigten Tasten als Eingabe unterstützt.\n");
        wprintf(L"- Wird viermal hintereinander eine ungültige Eingabe getätigt wird automatisch die %lsHilfe%ls aufgerufen.\n", TXT_INVERSE, END_STYLE);
        wprintf(L"- Wird bei angezeigter %lsHilfe%ls viermal hintereinander eine ungültige Eingabe getätigt wird das Programm automatisch beendet.\n", TXT_INVERSE, END_STYLE);
        wprintf(L"\n");

        
        wprintf(L"\n%lsHinzufügen von Veranstaltungen%ls\n", TXT_UNDERLINED, END_STYLE);
        wprintf(L"- Eine neue Veranstaltung kann von der %lsVeranstaltungsübersicht%ls oder der Anzeige des aktuelle %lsNotendurschnitts%ls aus hinzugefügt werden.\n", TXT_INVERSE, END_STYLE, TXT_INVERSE, END_STYLE);
        wprintf(L"- Es %lsmüssen%ls folgende Daten eingegeben werden:\n", TXT_BOLD, END_STYLE);
        wprintf(L"     - Name der Veranstaltung\n");
        wprintf(L"     - Modulgruppe\n");
        wprintf(L"     - Semester in dem die Veranstaltung besucht wird\n");
        wprintf(L"     - Leistungspunkte (LP)\n");
        wprintf(L"     - Optional: Erzielte Note (wird keine Note eingegeben wird der Status der Veranstaltung auf %lsausstehend%ls gesetzt).\n", TXT_YELLOW, END_STYLE);
        wprintf(L"\n");


        wprintf(L"\n%lsHinzufügen von Modulgruppen%ls\n", TXT_UNDERLINED, END_STYLE);
        wprintf(L"- Eine neue Veranstaltung kann von der %lsVeranstaltungsübersicht%ls oder der Anzeige des aktuelle %lsNotendurschnitts%ls aus hinzugefügt werden.\n", TXT_INVERSE, END_STYLE, TXT_INVERSE, END_STYLE);
        wprintf(L"- Es %lsmüssen%ls folgende Daten eingegeben werden:\n", TXT_BOLD, END_STYLE);
        wprintf(L"     - Name der Modulgruppe\n");
        wprintf(L"     - Innerhalb der Modulgruppe zu erreichende Leistungspunkte\n");
        wprintf(L"\n");


        wprintf(L"\n\n");
        wprintf(L"  %lsOptionen%ls                        %lsTaste%ls                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        wprintf(L"  Veranstaltungsübersicht           [v]\n");
        wprintf(L"  Notendurchschnitt anzeigen        [d]\n");
        wprintf(L"\n");
        wprintf(L"  Programm beenden                  [q]                    \n");
        wprintf(L"\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
}



void print_endscreen(void)
{
        clear_display();
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                               %ls%lsDas Programm wurde beendet!%ls                            │\n", TXT_INVERSE, TXT_GREEN, END_STYLE);
        wprintf(L"│                 Vielen Dank, dass Sie unseren Studienplaner genutzt haben.           │\n");
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────┘\n\n\n");

}





int print_averagescreen(int new_entry, struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod)
{ 
        clear_display(); 

        if (new_entry) {
                print_inputcompletescreen();      
        }

        // Berechnung des aktuellen Notendurchschnitts
        double average_grade = notendurchschnitt_po(ver, size_ver, mod, size_mod);

        wprintf(L"\n%ls Notendurchschnitt %ls\n\n", TXT_INVERSE, END_STYLE);
        wprintf(L"Berechnung nach PO: %ls\n\n", PO_2018);
        wprintf(L"      ╭───────────────────────────────────────────────────────╮\n");
        wprintf(L"      │         %lsAktueller Notendurchschnitt:%ls %ls%.1f%ls              │\n", TXT_GREEN, END_STYLE, TXT_UNDERLINED, average_grade, END_STYLE);
        wprintf(L"      ╰───────────────────────────────────────────────────────╯\n\n\n");

        wprintf(L"  %lsOptionen%ls                        %lsTaste%ls                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        wprintf(L"  Veranstaltungsübersicht           [v]                    Veranstaltung hinzufügen          [n]\n");
        wprintf(L"                                                           Modulgruppe hinzufügen            [m]\n");
        wprintf(L"                                                           Veranstaltung bearbeiten          [b]\n");
        wprintf(L"\n");
        wprintf(L"  Hilfe                             [h]                    \n");
        wprintf(L"  Programm beenden                  [q]                    \n");
        wprintf(L"\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        return 0;
}





void clear_display(void)
{
        wprintf(L"\033[0;0H");
        for (int i = 0; i < 50; ++i) {
                for (int a = 0; a < 400; ++a) {
                        wprintf(L" ");
                }
                wprintf(L"\n");
        }
        wprintf(L"\033[0;0H\n\n\n");

}



int print_addverscreen(struct Veranstaltung **ver, size_t *size_ver, struct Modulgruppe **mod, size_t *size_mod)
{
        clear_display();

        int status = 1;
        struct Veranstaltung new_ver;

        // Name einlesen
        wprintf(L"\n%lsNeue Veranstaltung hinzufügen%ls\n\n", TXT_UNDERLINED, END_STYLE);
        wprintf(L"Name der Veranstaltung:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        wchar_t *p = NULL;
        status = read_string(&p);
        switch (status) {
                case VALID_USER_INPUT:
                        new_ver.name = p;
                        break;
                case INVALID_USER_INPUT:
                        return INVALID_USER_INPUT;
                case BUFFER_ERROR:
                        return BUFFER_ERROR;
                case INVALID_FUNCTION_INPUT:
                        return INVALID_FUNCTION_INPUT;
                case MEM_ALLOC_ERROR:
                        return MEM_ALLOC_ERROR;
        }
        status = 1;

        
        // Modulgruppe einlesen
        clear_display();
        wprintf(L"\n\nDie Veranstaltung muss einer Modulgruppe hinzugefügt werden!\n");
        wprintf(L"\n%lsVorhandene Modulgruppen%ls                   \n\n", TXT_INVERSE, END_STYLE);
        wprintf(L"%lsName%ls", TXT_UNDERLINED, END_STYLE);
        for (int i = 0; i < 81; ++i) {
                wprintf(L" ");
        }
        wprintf(L"%lsNummer%ls\n", TXT_UNDERLINED, END_STYLE);
        sort_mod_by_alpha(*mod, *size_mod);
        for (int i = 0; i < (int) *size_mod; ++i) {
                wprintf(L"%ls", (*mod)[i].name);
                int counter = 0;
                while((*mod)[i].name[counter] != L'\0') {
                        ++counter;
                }
                counter = 90 - counter;
                for (int a = 0; a < counter; ++a) {
                        wprintf(L" ");
                }
                wprintf(L"%i\n", i);
        }

        wprintf(L"\n\nZu bestehender Modulgruppe hinzufügen: [h]\nZu neuer Modulgruppe hinzufügen [n]\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        status = read_command(ADD_TO_OLD_OR_NEW_MOD, SIZE_ADD_TO_OLD_OR_NEW_MOD);
        int stat_1 = 1;
        int width_name_mod = 0;
        switch (status) {
                case L'h':
                        wprintf(L"\nNummer der Modulgruppe:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                        int zahl = -1;
                        stat_1 = read_number_in_bound(0, (int) ((*size_mod) - 1), &zahl);
                        switch (stat_1) {
                                case VALID_USER_INPUT:

                                        clear_display();
                                        width_name_mod = 0;
                                        while ((*mod)[zahl].name[width_name_mod] != '\0') {
                                                ++width_name_mod;
                                        }
                                        wprintf(L"      ╭──────────────────────────────────────────────────────────────────────────────╮\n");
                                        wprintf(L"      │                        %ls Die Eingabe war erfolgreich %ls                         │\n", TXT_GREEN, END_STYLE);
                                        wprintf(L"      │                                                                              │\n");
                                        wprintf(L"      │          Die Veranstaltung wurde zu folgender Modulgruppe hinzugefügt:       │\n");
                                        wprintf(L"      │          %ls%ls%ls", TXT_UNDERLINED, (*mod)[zahl].name, END_STYLE);
                                        for (int i = 0; i < 68 - width_name_mod; ++i) {
                                                wprintf(L" ");
                                        }
                                        wprintf(L"│\n");
                                        wprintf(L"      │                                                                              │\n");
                                        wprintf(L"      ╰──────────────────────────────────────────────────────────────────────────────╯\n\n\n\n");


                                        new_ver.modulgruppenindex = zahl;
                                        break;
                                case INVALID_USER_INPUT:
                                        return INVALID_USER_INPUT;
                                case BUFFER_ERROR:
                                        return BUFFER_ERROR;
                                case INVALID_FUNCTION_INPUT:
                                        return INVALID_FUNCTION_INPUT;
                                case MEM_ALLOC_ERROR:
                                        return MEM_ALLOC_ERROR;
                                }
                        break;
                case L'n':
                        stat_1 = print_addmodscreen(mod, size_mod);
                        switch (stat_1) {
                                case VALID_USER_INPUT:
                                        clear_display();
                                        width_name_mod = 0;
                                        while ((*mod)[(*size_mod) - 1].name[width_name_mod] != '\0') {
                                                ++width_name_mod;
                                        }
                                        wprintf(L"      ╭──────────────────────────────────────────────────────────────────────────────╮\n");
                                        wprintf(L"      │                         %ls Die Eingabe war erfolgreich %ls                        │\n", TXT_GREEN, END_STYLE);
                                        wprintf(L"      │                                                                              │\n");
                                        wprintf(L"      │                   Es wurde eine neue Modulgruppe angelegt:                   │\n");
                                        wprintf(L"      │                   %ls%ls%ls", TXT_UNDERLINED, (*mod)[(*size_mod) - 1].name, END_STYLE);
                                        for (int i = 0; i < 59 - width_name_mod; ++i) {
                                                wprintf(L" ");
                                        }
                                        wprintf(L"│\n");
                                        wprintf(L"      │                                                                              │\n");
                                        wprintf(L"      │            Die Veranstaltung wurde zu dieser Modulgruppe hinzugefügt         │\n");
                                        wprintf(L"      │                                                                              │\n");
                                        wprintf(L"      ╰──────────────────────────────────────────────────────────────────────────────╯\n\n\n\n");

                                        new_ver.modulgruppenindex = (*size_mod) - 1;
                                        break;
                                case INVALID_USER_INPUT:
                                        return INVALID_USER_INPUT;
                                case BUFFER_ERROR:
                                        return BUFFER_ERROR;
                                case INVALID_FUNCTION_INPUT:
                                        return INVALID_FUNCTION_INPUT;
                                }
                        break;

                case INVALID_USER_INPUT:
                        return INVALID_USER_INPUT;
                case BUFFER_ERROR:
                        return BUFFER_ERROR;
                case INVALID_FUNCTION_INPUT:
                        return INVALID_FUNCTION_INPUT;
        }
        wprintf(L"%lsWeitere Daten der Veranstaltung:%ls", TXT_UNDERLINED, END_STYLE);
        status = 1;




        // Semester + Jahreszeit einlesen:
        // WS oder SS einlesen
        wprintf(L"\n\nSemester:\nWintersemester [w]   Sommersemester [s]\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        status = read_command(INPUT_WS_OR_SS, SIZE_INPUT_WS_OR_SS);
        switch (status) {
                case L'w':
                        new_ver.semester.jahreszeit = Winter;
                        break;
                case L's':
                        new_ver.semester.jahreszeit = Sommer;
                        break;
                case INVALID_USER_INPUT:
                        return INVALID_USER_INPUT;
                case BUFFER_ERROR:
                        return BUFFER_ERROR;
                case INVALID_FUNCTION_INPUT:
                        return INVALID_FUNCTION_INPUT;
        }
        status = 1;
        // Jahr einlesen
        wprintf(L"\n\nJahreszahl des Semesterbeginns:\nBeispiel:\n  - 2025 für WS 2025/2026\n  - 2026 für SS 2026\n");
        wprintf(L"%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        int jahr = -1;
        status = read_jahr(&jahr);
        switch (status) {
                case VALID_USER_INPUT:
                        new_ver.semester.jahr = jahr;
                        break;
                case INVALID_USER_INPUT:
                        return INVALID_USER_INPUT;
                case BUFFER_ERROR:
                        return BUFFER_ERROR;
                case INVALID_FUNCTION_INPUT:
                        return INVALID_FUNCTION_INPUT;
        }
        status = 1;




        // LP einlesen
        wprintf(L"\n\nLeistungspunkte: \n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        int lp = -1;
        status = read_lp(&lp);
        switch (status) {
                case VALID_USER_INPUT:
                        new_ver.lp = lp;
                        break;
                case INVALID_USER_INPUT:
                        return INVALID_USER_INPUT;
                case BUFFER_ERROR:
                        return BUFFER_ERROR;
                case INVALID_FUNCTION_INPUT:
                        return INVALID_FUNCTION_INPUT;
        }
        status = 1;



        // Note einlesen
        wprintf(L"\n\nNote hinzufügen:\n   Ja [j]   Nein [n]\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        status = read_command(INPUT_YES_OR_NO, SIZE_INPUT_YES_OR_NO);
        switch (status) {
                case L'j':
                        wprintf(L"\n\n    Note eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                        double note = -1.0;
                        status = read_note(&note);
                        switch (status) {
                                case INVALID_USER_INPUT:
                                        return INVALID_USER_INPUT;
                                case BUFFER_ERROR:
                                        return BUFFER_ERROR;
                                case INVALID_FUNCTION_INPUT:
                                        return INVALID_FUNCTION_INPUT;
                        }
                        new_ver.note = note;
                        if (note <= 4.0) {
                                new_ver.state = Bestanden;
                        } else {
                                new_ver.state = NichtBestanden;
                        }
                        break;
                case L'n':
                        new_ver.note = note;
                        new_ver.state = Ausstehend;
                        break;
                case INVALID_USER_INPUT:
                        return INVALID_USER_INPUT;
                case BUFFER_ERROR:
                        return BUFFER_ERROR;
                case INVALID_FUNCTION_INPUT:
                        return INVALID_FUNCTION_INPUT;
        }
        status = 1;





        // Dynamisches Array der Veranstaltungen vergrößern und neue Veranstaltung hinzufügen
        size_t new_size_ver = *size_ver + 1;
        struct Veranstaltung *ver_p;
        ver_p = realloc(*ver, (new_size_ver) * sizeof(struct Veranstaltung));
        if (ver_p == NULL) {
                return MEM_ALLOC_ERROR;
        } 
        *size_ver = new_size_ver;

        size_t temp = *size_ver;
        --temp;
        ver_p[temp].name = new_ver.name;
        ver_p[temp].note = new_ver.note;
        ver_p[temp].lp = new_ver.lp;
        ver_p[temp].modulgruppenindex = new_ver.modulgruppenindex;
        ver_p[temp].semester.jahr = new_ver.semester.jahr;
        ver_p[temp].semester.jahreszeit = new_ver.semester.jahreszeit;
        ver_p[temp].state = new_ver.state;

        *ver = ver_p;
        return VALID_USER_INPUT;
        




        
      

}





void print_inputcompletescreen(void)
{
        wprintf(L"      ╭───────────────────────────────────────────────────────╮\n");
        wprintf(L"      │         %ls Die Eingabe war erfolgreich %ls                 │\n", TXT_GREEN, END_STYLE);
        wprintf(L"      ╰───────────────────────────────────────────────────────╯\n\n\n");
        
}  




int print_addmodscreen(struct Modulgruppe **mod, size_t *size_mod)
{
        clear_display();

        int status = 1;
        struct Modulgruppe new_mod;

        // Name einlesen
        wprintf(L"\n%lsNeue Modulgruppe hinzufügen%ls\n\n", TXT_UNDERLINED, END_STYLE);
        wprintf(L"Name der Modulgruppe:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        wchar_t *p = NULL;
        status = read_string(&p);
        switch (status) {
                case VALID_USER_INPUT:
                        new_mod.name = p;
                        break;
                case INVALID_USER_INPUT:
                        return INVALID_USER_INPUT;
                case BUFFER_ERROR:
                        return BUFFER_ERROR;
                case INVALID_FUNCTION_INPUT:
                        return INVALID_FUNCTION_INPUT;
                case MEM_ALLOC_ERROR:
                        return MEM_ALLOC_ERROR;
        }
        status = 1;

        // Neuen Modulgruppenindex speichern
        new_mod.modulgruppenindex = *size_mod;



        // LP einlesen
        wprintf(L"\n\nLeistungspunkte in dieser Modulgruppe: \n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        int lp = -1;
        status = read_lp(&lp);
        switch (status) {
                case VALID_USER_INPUT:
                        new_mod.lp_todo = lp;
                        break;
                case INVALID_USER_INPUT:
                        return INVALID_USER_INPUT;
                case BUFFER_ERROR:
                        return BUFFER_ERROR;
                case INVALID_FUNCTION_INPUT:
                        return INVALID_FUNCTION_INPUT;
        }
        status = 1;






        // Dynamisches Array der Veranstaltungen vergrößern und neue Veranstaltung hinzufügen
        size_t new_size_mod = *size_mod + 1;
        struct Modulgruppe *mod_p;
        mod_p = realloc(*mod, (new_size_mod) * sizeof(struct Modulgruppe));
        if (mod_p == NULL) {
                return MEM_ALLOC_ERROR;
        } 
        *size_mod = new_size_mod;

        size_t temp = *size_mod;
        --temp;
        mod_p[temp].name = new_mod.name;
        mod_p[temp].lp_todo = new_mod.lp_todo;
        mod_p[temp].modulgruppenindex = new_mod.modulgruppenindex;
        *mod = mod_p;
        return VALID_USER_INPUT; 
}


void print_buffer_error_screen(void)
{
        clear_display();
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                                    %ls%lsPUFFERFEHLER!%ls                                     │\n", TXT_INVERSE, TXT_RED, END_STYLE);
        wprintf(L"│         Das Programm wird beendet. Für einen weiteren Versuch bitte neu starten.     │\n");
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────┘\n\n\n");

}

void print_memalloc_error_screen(void)
{
        clear_display();
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                     %ls%lsDynamische Speicherreservierung fehlgeschlagen!%ls                  │\n", TXT_INVERSE, TXT_RED, END_STYLE);
        wprintf(L"│         Das Programm wird beendet. Für einen weiteren Versuch bitte neu starten.     │\n");
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────┘\n\n\n");

}


void print_wrong_command_screen(void)
{
        wprintf(L"\n");
        wprintf(L"┌─────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                         %ls%lsFALSCHE EINGABE!%ls                            │\n", TXT_INVERSE, TXT_RED, END_STYLE);
        wprintf(L"│               Bitte wählen Sie eine gültige Taste.                  │\n");
        wprintf(L"└─────────────────────────────────────────────────────────────────────┘");
        wprintf(L"\n\nGültige Taste wählen:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);

}


void print_wrong_string_screen(void)
{       
        wprintf(L"\n");
        wprintf(L"┌─────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                         %ls%lsFALSCHE EINGABE!%ls                            │\n", TXT_INVERSE, TXT_RED, END_STYLE);
        wprintf(L"│                    Bitte geben Sie einen Namen ein.                 │\n");
        wprintf(L"└─────────────────────────────────────────────────────────────────────┘");
        wprintf(L"\n\nName:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);

}


void print_wrong_function_input_screen(void)
{
        clear_display();
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                                 %ls%lsPROGRAMMFEHLER!%ls                                      │\n", TXT_INVERSE, TXT_RED, END_STYLE);
        wprintf(L"│                     Ungültige Eingabedaten der aufgerufenen Funktion.                │\n");
        wprintf(L"│         Das Programm wird beendet. Für einen weiteren Versuch bitte neu starten.     │\n");
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────┘\n\n\n");

}


void print_loaddata_error_screen(wchar_t *error_message)
{
        int len = 0;
        while (error_message[len] != L'\0') {
                ++len;
        }
        
        wprintf(L"\n\n");
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                                 %ls%lsSPEICHERFEHLER!%ls                                      │\n", TXT_INVERSE, TXT_RED, END_STYLE);
        wprintf(L"│                   Die Daten konnten nicht korrekt geladen werden.                    │\n");
        wprintf(L"│                                                                                      │\n");
        wprintf(L"│ %lsError message:%ls %ls", TXT_RED, END_STYLE, error_message);
        for (int i = 0; i < 70 - len; ++i) {
                wprintf(L" ");
        }
        wprintf(L"│\n");
        wprintf(L"│                                                                                      │\n");
        wprintf(L"│ %lsOptionen%ls                                                        %lsTaste%ls                │\n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        wprintf(L"│ Laden der Daten erneut versuchen                                  [r]                │\n");
        wprintf(L"│ Neue Datei erstellen (%lsbisherige Daten werden %lsgelöscht%ls)            [n]                │\n", TXT_BOLD, TXT_RED, END_STYLE);
        wprintf(L"│ Programm beenden                                                  [q]                │\n");
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────┘\n");
        wprintf(L"\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);

}




void print_loaddata_complete_screen(void)
{
        wprintf(L"\n\n");
        wprintf(L"      ╭───────────────────────────────────────────────────────╮\n");
        wprintf(L"      │       %ls Laden der Daten erfolgreich abgeschlossen! %ls    │\n", TXT_GREEN, END_STYLE);
        wprintf(L"      ╰───────────────────────────────────────────────────────╯\n\n");

}


void print_newsavefile_created(void)
{
        clear_display();
        wprintf(L"      ╭───────────────────────────────────────────────────────╮\n");
        wprintf(L"      │           %ls Es wurde eine neue Datei erstellt! %ls        │\n", TXT_GREEN, END_STYLE);
        wprintf(L"      ╰───────────────────────────────────────────────────────╯\n\n\n");

}

void print_no_ver_saved_screen(void)
{
        wprintf(L"\n\n");
        wprintf(L"      ╭─────────────────────────────────────────────────────────────╮\n");
        wprintf(L"      │        %ls Es wurde noch keine Veranstaltung hinzugefügt! %ls     │\n", TXT_GREEN, END_STYLE);
        wprintf(L"      ╰─────────────────────────────────────────────────────────────╯\n");

}



void print_savedata_complete_screen(void)
{
        wprintf(L"\n\n");
        wprintf(L"      ╭───────────────────────────────────────────────────────╮\n");
        wprintf(L"      │     %ls Speichern der Daten erfolgreich abgeschlossen! %ls  │\n", TXT_GREEN, END_STYLE);
        wprintf(L"      ╰───────────────────────────────────────────────────────╯\n\n");
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                               %ls%lsDas Programm wurde beendet!%ls                            │\n", TXT_INVERSE, TXT_GREEN, END_STYLE);
        wprintf(L"│                 Vielen Dank, dass Sie unseren Studienplaner genutzt haben.           │\n");
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────┘\n\n\n");


}



void print_savedata_error_screen(wchar_t *error_message)
{
        int len = 0;
        while (error_message[len] != L'\0') {
                ++len;
        }

        wprintf(L"\n\n");
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                                 %ls%lsSPEICHERFEHLER!%ls                                      │\n", TXT_INVERSE, TXT_RED, END_STYLE);
        wprintf(L"│                 Die Daten konnten nicht korrekt gespeichert werden.                  │\n");
        wprintf(L"│                                                                                      │\n");
        wprintf(L"│ %lsError message:%ls %ls", TXT_RED, END_STYLE, error_message);
        for (int i = 0; i < 70 - len; ++i) {
                wprintf(L" ");
        }
        wprintf(L"│\n");
        wprintf(L"│                                                                                      │\n");
        wprintf(L"│ %lsOptionen%ls                                          %lsTaste%ls                              │\n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        wprintf(L"│ Speichern der Daten erneut versuchen                [r]                              │\n");
        wprintf(L"│ Programm beenden %lsohne%ls zu speichern                  [q]                              │\n", TXT_RED, END_STYLE);
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────┘\n");
        wprintf(L"\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);

}



void free_all(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod)
{
        // Veranstaltungsname freigeben
        for (int i = 0; i < (int) size_ver; ++i) {
                free(ver[i].name);
        }
        
        // Speicher der Veranstaltungen freigeben
        free(ver);

        // Modulgruppennamen freigeben
        for (int i = 0; i < (int) size_mod; ++i) {
                free(mod[i].name);
        }

        // Speicher der Modulgruppen freigeben
        free(mod);
}