#include "text_formatting.h"
#include "../../midend/data.h"
#include <stdio.h>
#include <string.h>


void print_welcomescreen()
{
        printf("┌───────────────────────────────────────────────────────┐\n");
        printf("│                                                       │\n");
        printf("│%s     |  |  |  |    |/  /\\  |\\/|  |\\/|  |=  |\\ |  │     %s│\n", TXT_GREEN, END_STYLE);
        printf("│%s     |/\\|  |  |__  |\\  \\/  |  |  |  |  |=  | \\|  .     %s│\n", TXT_GREEN, END_STYLE);
        printf("│                                                       │\n");
        printf("│       %sDies ist ihr persönlicher Studienplaner%s         │\n", "\033[31;44m", "\033[0m");
        printf("│                                                       │\n");
        printf("└───────────────────────────────────────────────────────┘\n");
        printf("  %sOptionen%s                        %sTaste%s                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        printf("  Veranstaltungsübersicht           [v]                    \n");
        printf("  Hilfe                             [h]                    \n");
        printf("  Programm beenden                  [q]                    \n");
        printf("\n%s>>>%s ", TXT_INVERSE, END_STYLE);
        
}


int print_overviewscreen(struct Veranstaltung *ver, size_t size_ver, struct Modulgruppe *mod, size_t size_mod, int view_type)
{
        


        clear_display();
        
        printf("%sVeranstaltungsübersicht%s\n\n", TXT_INVERSE, END_STYLE);

        // Übersicht nach Semester geordnet ausgeben
        if (view_type == 1) {
                print_overview_by_time(ver, size_ver);
        }
        

        


        printf("%s%s\n\n\nÜbersicht%s\n\n", TXT_INVERSE, TXT_RED, END_STYLE);
        printf("  %sOptionen%s                        %sTaste%s                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        printf("  Ansicht ändern                    [a]                    Veranstaltung hinzufügen          [n]\n");
        printf("  (Sortierung nach Modulgruppen)                           Modulgruppe hinzufügen            [m]\n");
        printf("  Notendurchschnitt anzeigen        [d]                    Veranstaltung bearbeiten          [b]\n");
        printf("\n");
        printf("  Hilfe                             [h]                    \n");
        printf("  Programm beenden                  [q]                    \n");
        printf("\n%s>>>%s ", TXT_INVERSE, END_STYLE);
        
        return 0;
}




void print_helpscreen()
{
        clear_display();

        printf("\n%s%sDas hier ist die Hilfeseite%s\n", TXT_RED, TXT_INVERSE, END_STYLE);
        printf("  %sOptionen%s                        %sTaste%s                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        printf("  Veranstaltungsübersicht           [v]\n");
        printf("  Notendurchschnitt anzeigen        [d]\n");
        printf("\n");
        printf("  Programm beenden                  [q]                    \n");
        printf("\n%s>>>%s ", TXT_INVERSE, END_STYLE);
}



void print_endscreen()
{
        clear_display();
        
        printf("Vielen Dank, dass Sie unser Programm benutzt haben.\nAuf Wiedersehen :-)\n");
}




int print_averagescreen()
{
        clear_display();       


        printf("\n%s%sHier sieht man den Notendurchschnitt%s\n", TXT_RED, TXT_INVERSE, END_STYLE);
        printf("  %sOptionen%s                        %sTaste%s                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        printf("  Veranstaltungsübersicht           [v]                    Veranstaltung hinzufügen          [n]\n");
        printf("                                                           Modulgruppe hinzufügen            [m]\n");
        printf("                                                           Veranstaltung bearbeiten          [b]\n");
        printf("\n");
        printf("  Hilfe                             [h]                    \n");
        printf("  Programm beenden                  [q]                    \n");
        printf("\n%s>>>%s ", TXT_INVERSE, END_STYLE);
        return 0;
}




void clear_display()
{
        printf("\033[0;0H");
        for (int i = 0; i < 50; ++i) {
                for (int a = 0; a < 400; ++a) {
                        printf(" ");
                }
                printf("\n");
        }
        printf("\033[0;0H");

}




void print_overview_by_time(struct Veranstaltung *ver, size_t size_ver)
{
        int last_index = -1;
        int last_time = -1;
        char v[] = "bestanden";
        char x[] = "nicht bestanden";
        char y[] = "ausstehend";



        for (size_t i = 0; i < size_ver; ++i) {

                // AUFRUF FUNKTION ZUR SORTIERUNG DER VERANSTALTUNGEN NACH ZEIT => Adrian
                // UND
                // AUFRUF FUNKTION ZUR ALPHABETISCHEN SORTIERUNG DER VERANSTALTUNGEN => Adrian
                if (ver[i].semester.jahr != last_time) {
                        if (ver[i].semester.jahreszeit == Winter) {
                                printf("\n\n%sWS %i/%i%s\n", TXT_UNDERLINED, ver[i].semester.jahr, ver[i].semester.jahr + 1, END_STYLE);
                                printf("────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\n");
                        } else {
                                printf("\n\n%sSS %i%s\n", TXT_UNDERLINED, ver[i].semester.jahr, END_STYLE);
                                printf("────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\n");
                        }
                        last_time = ver[i].semester.jahr;
                }

                // Veranstaltungsnamen ausgeben
                printf("%s", ver[i].name);
                int counter = 0;
                while (ver[i].name[counter] != '\0') {
                        ++counter;
                }
                counter = 90 - counter;
                for (int a = 0; a < counter; ++a) {
                        printf(" ");
                }

                // Restliche Daten der Veranstaltung ausgeben
                switch(ver[i].state) {
                        case Bestanden:
                                printf("        %15s     %i    %.1f\n", v, ver[i].lp, ver[i].note);
                                break;
                        case NichtBestanden:
                                printf("        %s%15s%s     %i    %s%.1f%s\n", TXT_RED, x, END_STYLE, ver[i].lp, TXT_RED, ver[i].note, END_STYLE);
                                break;
                        case Ausstehend:
                                printf("        %s%15s%s     %i    %s/%s\n", TXT_YELLOW, y, END_STYLE, ver[i].lp, TXT_YELLOW, END_STYLE);
                                break;
                }
        }
}