#include "text_formatting.h"
#include "../user_input/user_input.h"
#include "../../midend/data.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>


void print_welcomescreen()
{
        wprintf(L"┌───────────────────────────────────────────────────────┐\n");
        wprintf(L"│                                                       │\n");
        wprintf(L"│%ls     |  |  |  |    |/  /\\  |\\/|  |\\/|  |=  |\\ |  │     %ls│\n", TXT_GREEN, END_STYLE);
        wprintf(L"│%ls     |/\\|  |  |__  |\\  \\/  |  |  |  |  |=  | \\|  .     %ls│\n", TXT_GREEN, END_STYLE);
        wprintf(L"│                                                       │\n");
        wprintf(L"│       %lsDies ist ihr persönlicher Studienplaner%ls         │\n", TXT_INVERSE, END_STYLE);
        wprintf(L"│                                                       │\n");
        wprintf(L"└───────────────────────────────────────────────────────┘\n");
        wprintf(L"  %lsOptionen%ls                        %lsTaste%ls                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
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


        wprintf(L"%lsVeranstaltungsübersicht%ls\n\n", TXT_INVERSE, END_STYLE);
        

        // Übersicht nach Semester geordnet ausgeben
        if (view_type == 1) {
                print_overview_by_time(ver, size_ver);
        } else if (view_type == 2) {
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









void print_helpscreen()
{
        clear_display();

        wprintf(L"\n%ls%lsDas hier ist die Hilfeseite%ls\n", TXT_RED, TXT_INVERSE, END_STYLE);
        wprintf(L"  %lsOptionen%ls                        %lsTaste%ls                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        wprintf(L"  Veranstaltungsübersicht           [v]\n");
        wprintf(L"  Notendurchschnitt anzeigen        [d]\n");
        wprintf(L"\n");
        wprintf(L"  Programm beenden                  [q]                    \n");
        wprintf(L"\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
}







void print_endscreen()
{
        clear_display();
        
        wprintf(L"Vielen Dank, dass Sie unser Programm benutzt haben.\nAuf Wiedersehen :-)\n");
}









int print_averagescreen(int new_entry)
{ 
        clear_display(); 

        if (new_entry) {
                print_inputcompletescreen();      
        }

        wprintf(L"\n%ls%lsHier sieht man den Notendurchschnitt%ls\n", TXT_RED, TXT_INVERSE, END_STYLE);
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








void clear_display()
{
        wprintf(L"\033[0;0H");
        for (int i = 0; i < 50; ++i) {
                for (int a = 0; a < 400; ++a) {
                        wprintf(L" ");
                }
                wprintf(L"\n");
        }
        wprintf(L"\033[0;0H");

}









void print_overview_by_time(struct Veranstaltung *ver, size_t size_ver)
{
        struct Semester last_time;
        last_time.jahr = -1;
        last_time.jahreszeit = -1;
        wchar_t v[] = L"bestanden";
        wchar_t x[] = L"nicht bestanden";
        wchar_t y[] = L"ausstehend";



        for (size_t i = 0; i < size_ver; ++i) {

                // AUFRUF FUNKTION ZUR SORTIERUNG DER VERANSTALTUNGEN NACH ZEIT => Adrian
                // UND
                // AUFRUF FUNKTION ZUR ALPHABETISCHEN SORTIERUNG DER VERANSTALTUNGEN => Adrian
                if (ver[i].semester.jahr != last_time.jahr || ver[i].semester.jahreszeit != last_time.jahreszeit) {
                        if (ver[i].semester.jahreszeit == Winter) {
                                wprintf(L"\n\n%lsWS %i/%i%ls\n", TXT_UNDERLINED, ver[i].semester.jahr, ver[i].semester.jahr + 1, END_STYLE);
                                wprintf(L"────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\n");
                        } else {
                                wprintf(L"\n\n%lsSS %i%ls\n", TXT_UNDERLINED, ver[i].semester.jahr, END_STYLE);
                                wprintf(L"────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\n");
                        }
                        last_time.jahr = ver[i].semester.jahr;
                        last_time.jahreszeit = ver[i].semester.jahreszeit;
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
                                wprintf(L"        %ls%15ls%ls     %i    %.1f\n", TXT_GREEN, v, END_STYLE, ver[i].lp, ver[i].note);
                                break;
                        case NichtBestanden:
                                wprintf(L"        %ls%15ls%ls     %i    %ls%.1f%ls\n", TXT_RED, x, END_STYLE, ver[i].lp, TXT_RED, ver[i].note, END_STYLE);
                                break;
                        case Ausstehend:
                                wprintf(L"        %ls%15ls%ls     %i    %ls/%ls\n", TXT_YELLOW, y, END_STYLE, ver[i].lp, TXT_YELLOW, END_STYLE);
                                break;
                }
        }
}





void print_overview_by_mod(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod)
{
        int last_index = -1;
        wchar_t v[] = L"bestanden";
        wchar_t x[] = L"nicht bestanden";
        wchar_t y[] = L"ausstehend";

        



        for (size_t i = 0; i < size_ver; ++i) {

                // AUFRUF FUNKTION ZUR SORTIERUNG DER VERANSTALTUNGEN NACH MODULGRUPPE => Adrian
                // UND
                // AUFRUF FUNKTION ZUR ALPHABETISCHEN SORTIERUNG DER VERANSTALTUNGEN => Adrian
                if (ver[i].modulgruppenindex != last_index) {
                        size_t a = 0;
                        while (a < size_mod) {
                                if (ver[i].modulgruppenindex == mod[a].modulgruppenindex) {
                                        break;
                                }
                                ++a;
                        }
                        wprintf(L"\n\n%ls%ls%ls\n", TXT_UNDERLINED, mod[a].name, END_STYLE);
                        wprintf(L"───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\n");
                        
                        last_index = ver[i].modulgruppenindex;
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
                                wprintf(L"        %ls%15ls%ls     %i    %.1f\n", TXT_GREEN, v, END_STYLE, ver[i].lp, ver[i].note);
                                break;
                        case NichtBestanden:
                                wprintf(L"        %ls%15ls%ls     %i    %ls%.1f%ls\n", TXT_RED, x, END_STYLE, ver[i].lp, TXT_RED, ver[i].note, END_STYLE);
                                break;
                        case Ausstehend:
                                wprintf(L"        %ls%15ls%ls     %i    %ls/%ls\n", TXT_YELLOW, y, END_STYLE, ver[i].lp, TXT_YELLOW, END_STYLE);
                                break;
                }
        }



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
        switch (status) {
                case L'h':
                        wprintf(L"\nNummer der Modulgruppe:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                        int zahl = -1;
                        int stat_1 = 1;
                        stat_1 = read_number_in_bound(0, (int) ((*size_mod) - 1), &zahl);
                        switch (stat_1) {
                                case VALID_USER_INPUT:
                                        wprintf(L"Die Veranstaltung wurde zu folgender Modulgruppe hinzugefügt:\n");
                                        wprintf(L"%ls", (*mod)[zahl].name);
                                        new_ver.modulgruppenindex = zahl;
                                        break;
                                case INVALID_USER_INPUT:
                                        return INVALID_USER_INPUT;
                                case BUFFER_ERROR:
                                        return BUFFER_ERROR;
                                case INVALID_FUNCTION_INPUT:
                                        return INVALID_FUNCTION_INPUT;
                                }
                        break;

                case L'n':
                        int stat_2 = 1;
                        stat_2 = print_addmodscreen(mod, size_mod);
                        switch (stat_2) {
                                case VALID_USER_INPUT:
                                        wprintf(L"\nDie Veranstaltung wurde zu folgender Modulgruppe hinzugefügt:\n");
                                        wprintf(L"%ls", (*mod)[(*size_mod) - 1].name);
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
        wprintf(L"\n\nJahreszahl des Semesterbeginns:\n     Beispiel:\n     - 2025 für WS 2025/2026\n     - 2026 für SS 2026\n");
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



















void print_inputcompletescreen()
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
