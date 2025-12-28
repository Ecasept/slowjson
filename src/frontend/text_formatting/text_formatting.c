#include "text_formatting.h"
#include "../../midend/data.h"
#include <stdio.h>
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


int print_overviewscreen(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod, int view_type)
{
        


        clear_display();
        
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




int print_averagescreen()
{
        clear_display();       


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
        int last_time = -1;
        wchar_t v[] = L"bestanden";
        wchar_t x[] = L"nicht bestanden";
        wchar_t y[] = L"ausstehend";



        for (size_t i = 0; i < size_ver; ++i) {

                // AUFRUF FUNKTION ZUR SORTIERUNG DER VERANSTALTUNGEN NACH ZEIT => Adrian
                // UND
                // AUFRUF FUNKTION ZUR ALPHABETISCHEN SORTIERUNG DER VERANSTALTUNGEN => Adrian
                if (ver[i].semester.jahr != last_time) {
                        if (ver[i].semester.jahreszeit == Winter) {
                                wprintf(L"\n\n%lsWS %i/%i%ls\n", TXT_UNDERLINED, ver[i].semester.jahr, ver[i].semester.jahr + 1, END_STYLE);
                                wprintf(L"────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\n");
                        } else {
                                wprintf(L"\n\n%lsSS %i%ls\n", TXT_UNDERLINED, ver[i].semester.jahr, END_STYLE);
                                wprintf(L"────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\n");
                        }
                        last_time = ver[i].semester.jahr;
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