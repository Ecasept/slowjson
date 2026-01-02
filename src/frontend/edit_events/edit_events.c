#include "edit_events.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "../text_formatting/text_formatting.h"
#include "../user_input/user_input.h"
#include "../../midend/data.h"
#include "../../midend/mid.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>



int print_editver(struct Veranstaltung **ver, size_t *size_ver, struct Modulgruppe **mod, size_t *size_mod, int *current_page)
{
        int status_editver = -1;
        int selected_ver = -1;

        // Prüfung ob *ver mind. eine Veranstaltung enthält die bearbeitet werden kann
        if (*size_ver == 0) {
                status_editver = print_noverscreen();
                STANDARD_ERROR_HANDLING(status_editver);
                return VALID_USER_INPUT;
        }
        

        clear_display();

        // Übersicht der Veranstaltungen anzeigen => Veranstaltung zum bearbeiten wählen
        status_editver = select_ver(ver, size_ver, &selected_ver);
        STANDARD_ERROR_HANDLING(status_editver);
        clear_display();

        while(1) {
                // Prüfung ob *ver mind. eine Veranstaltung enthält die bearbeitet werden kann
                if (*size_ver == 0) {
                        status_editver = print_noverscreen();
                        STANDARD_ERROR_HANDLING(status_editver);
                        return VALID_USER_INPUT;
                }

                // Übersicht über ausgewählte Veranstaltung und Optionen anzeigen
                
                if (selected_ver != -1) {
                        print_selected_ver(*ver, *mod, *size_mod, selected_ver);
                } else {
                        print_eventdeleted();
                }
                print_options_editver();
                status_editver = read_command(INPUT_EDIT_VER, SIZE_INPUT_EDIT_VER);
                STANDARD_ERROR_HANDLING(status_editver);
                switch (status_editver) {
                        case L'v':
                                *current_page = CURR_PAGE_OV_SCR;
                                return VALID_USER_INPUT;
                        case L'c':
                                clear_display();
                                status_editver = select_ver(ver, size_ver, &selected_ver);
                                STANDARD_ERROR_HANDLING(status_editver);
                                clear_display();
                                break;
                        case L'h':
                                print_helpscreen();
                                *current_page = CURR_PAGE_HELP_SCR;
                                return VALID_USER_INPUT;
                        case L'l':
                                status_editver = delete_verscreen(ver, size_ver, mod, size_mod, &selected_ver);
                                clear_display(); 
                                STANDARD_ERROR_HANDLING(status_editver);
                                break;
                        case L'n':
                                status_editver = change_notescreen(ver, mod, size_mod, selected_ver);
                                STANDARD_ERROR_HANDLING(status_editver);
                                clear_display();
                                if (status_editver == CHANGES_SAVED) {
                                        print_notechangedscreen();
                                }
                                break;
                        case L'm':
                                status_editver = change_modscreen(ver, mod, size_mod, selected_ver);
                                STANDARD_ERROR_HANDLING(status_editver);
                                clear_display();
                                if (status_editver == CHANGES_SAVED) {
                                        print_modchangedscreen();
                                }
                                break;
                }
                        
        }


        
        
}










int select_ver(struct Veranstaltung **ver, size_t *size_ver, int *selected_ver)
{
        int stat_selectver = -1;
        int number = -1;


        // Kopfzeile
        wprintf(L"Bitte wählen Sie eine Veranstaltung die Sie bearbeiten möchten:\n\n");
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
                perror("ioctl");
        }
        int width = (int) (0.75 * w.ws_col);
        wprintf(L"Name");
        for (int i = 0; i < 86; ++i) {
                wprintf(L" ");
        }
        wprintf(L"Nummer\n");
        for (int i = 0; i < width; ++i) {
                wprintf(L"─");
        }

        
        // Veranstaltungen auflisten
        for (int i = 0; i < (int) (*size_ver); ++i) {
                wprintf(L"\n%ls", (*ver)[i].name);
                int counter = 0;
                while((*ver)[i].name[counter] != L'\0') {
                        ++counter;
                }
                counter = 90 - counter;
                for (int a = 0; a < counter; ++a) {
                        wprintf(L" ");
                }
                wprintf(L"%i", i);
        }

        wprintf(L"\n\nNummer:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        stat_selectver = read_number_in_bound(0, (*size_ver) - 1, &number);
        STANDARD_ERROR_HANDLING(stat_selectver);
        *selected_ver = number;
        return VALID_USER_INPUT;

}








void print_selected_ver(struct Veranstaltung *ver, struct Modulgruppe *mod, size_t size_mod, int selected_ver)
{
        int pos_mod = -1;
        int width_name_mod = 0;
        int width_name_ver = 0;
        int width_square = -1;
        int width_extra = 30;


        wprintf(L"Sie haben folgende Veranstaltung ausgewählt:\n\n");

        // Breite Name der Veranstaltung ermitteln
        while (ver[selected_ver].name[width_name_ver] != L'\0') {
                ++width_name_ver;
        }

        
        // Modulgruppe ermitteln und Breite des Namens der Modulgruppe ermitteln
        for (int i = 0; i < (int) size_mod; ++i) {
                if (mod[i].modulgruppenindex == ver[selected_ver].modulgruppenindex) {
                        pos_mod = i;
                }
        }
        while (mod[pos_mod].name[width_name_mod] != L'\0') {
                ++width_name_mod;
        }

        // Breite des Kastens festlegen
        if (width_name_ver >= width_name_mod) {
                width_square = width_name_ver + width_extra;
        } else {
                width_square = width_name_mod + width_extra;
        }

        wprintf(L"╭");
        for (int i = 0; i <= width_square; ++i) {
                wprintf(L"─");
        }
        wprintf(L"╮\n");

        

        // Namen Veranstaltung und Modulgruppe ausgeben
        wprintf(L"│Name:");
        STANDARD_SPACE(5);
        wprintf(L"%ls", ver[selected_ver].name);
        wprintf(L"\033[%iG", width_square + 3);
        wprintf(L"│\n");
        wprintf(L"│Modulgruppe:");
        STANDARD_SPACE(12);
        wprintf(L"%ls", mod[pos_mod].name);
        wprintf(L"\033[%iG", width_square + 3);
        wprintf(L"│\n");


        // Semester
        wprintf(L"│Semester:");
        STANDARD_SPACE(strlen("Semester:"));
        if (ver[selected_ver].semester.jahreszeit == Winter) {
                wprintf(L"WS %i/%i%ls", ver[selected_ver].semester.jahr, ver[selected_ver].semester.jahr + 1, END_STYLE);
        } else {
                wprintf(L"SS %i%ls", ver[selected_ver].semester.jahr, END_STYLE);                  
        }
        wprintf(L"\033[%iG", width_square + 3);
        wprintf(L"│\n");

        // LP
        wprintf(L"│LP:");
        STANDARD_SPACE(strlen("LP:"));
        wprintf(L"%i", ver[selected_ver].lp);
        wprintf(L"\033[%iG", width_square + 3);
        wprintf(L"│\n");
        
        // Note
        wprintf(L"│Note:");
        STANDARD_SPACE(strlen("Note:"));
        switch(ver[selected_ver].state) {
                case Bestanden:
                        wprintf(L"%.1f", ver[selected_ver].note);
                        break;
                case NichtBestanden:
                        wprintf(L"%ls%.1f%ls", TXT_RED, ver[selected_ver].note, END_STYLE);
                        break;
                case Ausstehend:
                        wprintf(L"%ls/%ls", TXT_YELLOW, END_STYLE);
                        break;
        }
        wprintf(L"\033[%iG", width_square + 3);
        wprintf(L"│\n");


        // Status
        wprintf(L"│Status:");
        STANDARD_SPACE(strlen("Status:"));
        switch(ver[selected_ver].state) {
                        case Bestanden:
                                wprintf(L"%ls%ls%ls", TXT_GREEN, BESTANDEN, END_STYLE);
                                break;
                        case NichtBestanden:
                                wprintf(L"%ls%ls%ls", TXT_RED, NICHT_BESTANDEN, END_STYLE);
                                break;
                        case Ausstehend:
                                wprintf(L"%ls%ls%ls", TXT_YELLOW, AUSSTEHEND, END_STYLE);
                                break;
                }

        wprintf(L"\033[%iG", width_square + 3);
        wprintf(L"│\n");


        wprintf(L"╰");
        for (int i = 0; i <= width_square; ++i) {
                wprintf(L"─");
        }
        wprintf(L"╯\n");

}










void print_options_editver(void)
{
        wprintf(L"\n\n\n  %lsOptionen%ls                        %lsTaste%ls                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        wprintf(L"  Veranstaltung löschen             [l]           Zurück zur Veranstaltungsübersicht:          [v]\n");
        wprintf(L"  Note ändern/hinzufügen            [n]           Andere Veranstaltung wählen:                 [c]\n");
        wprintf(L"  Modulgruppe ändern                [m]\n");
        wprintf(L"\n");
        wprintf(L"  Hilfe                             [h]                    \n");
        wprintf(L"\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
}







int delete_verscreen(struct Veranstaltung **ver, size_t *size_ver, struct Modulgruppe **mod, size_t *size_mod, int *selected_ver)
{
        int status_deleteverscreen = -1;
        clear_display();
        print_selected_ver(*ver, *mod, *size_mod, *selected_ver);
        wprintf(L"%ls%lsAchtung:%ls Möchten Sie diese Veranstaltung wirklich löschen?\n", TXT_RED, TXT_INVERSE, END_STYLE);
        wprintf(L"\nJa [j]   Nein [n]\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        status_deleteverscreen = read_command(INPUT_YES_OR_NO, SIZE_INPUT_YES_OR_NO);
        STANDARD_ERROR_HANDLING(status_deleteverscreen);
        switch (status_deleteverscreen) {
                case L'j':
                        // Veranstaltung selected_ver in ver löschen
                        for (int i = *selected_ver; i < ((int) *size_ver) - 1; ++i) {
                                (*ver)[i].name = (*ver)[i + 1].name;
                                (*ver)[i].note = (*ver)[i + 1].note;
                                (*ver)[i].lp = (*ver)[i + 1].lp;
                                (*ver)[i].modulgruppenindex = (*ver)[i + 1].modulgruppenindex;
                                (*ver)[i].semester.jahr = (*ver)[i + 1].semester.jahr;
                                (*ver)[i].semester.jahreszeit = (*ver)[i + 1].semester.jahreszeit;
                                (*ver)[i].state = (*ver)[i + 1].state;
                        }

                        // Dynam. reservierten Speicherbereich verkleinern
                        int new_size_ver = ((int) (*size_ver)) - 1;
                        struct Veranstaltung *ver_temp = realloc(*ver, (new_size_ver) * sizeof(struct Veranstaltung));
                        if (ver_temp == NULL && (new_size_ver) != 0) {
                                return MEM_ALLOC_ERROR;
                        }
                        *size_ver = new_size_ver;
                        *ver = ver_temp; 
                        
                        // selected_ver auf -1 setzen, da die gelöschte Veranstaltung nicht mehr ausgewählt sein kann
                        *selected_ver = -1;                 
                        return VALID_USER_INPUT;

                case L'n':
                        return VALID_USER_INPUT;
        }
        return INVALID_FUNCTION_INPUT;
}


void print_eventdeleted(void)
{
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                                %ls%lsDie Veranstaltung wurde gelöscht!%ls                                 │\n", TXT_RED, TXT_INVERSE, END_STYLE);
        wprintf(L"│     Bitte wählen Sie eine andere Veranstaltung oder rufen Sie die Veranstaltungsübersicht auf    │\n");
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────────────────┘\n");
}

int print_noverscreen(void)
{
        clear_display();
        int status_noverscreen = -1;

        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                                    %ls%lsOption nicht verfügbar!%ls                                       │\n", TXT_RED, TXT_INVERSE, END_STYLE);
        wprintf(L"│                           Bitte erstellen Sie zunächst eine neue Veranstaltung                   │\n");
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────────────────┘\n");

        wprintf(L"\n\n  %lsOptionen%ls                              %lsTaste%ls                  \n", TXT_UNDERLINED, END_STYLE, TXT_UNDERLINED, END_STYLE);
        wprintf(L"  Zurück zum letzten Bildschirm           [z]\n\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        status_noverscreen = read_command(INPUT_BACK_TO_OV, SIZE_INPUT_BACK_TO_OV);
        STANDARD_ERROR_HANDLING(status_noverscreen);
        return VALID_USER_INPUT;      
}



int change_notescreen(struct Veranstaltung **ver, struct Modulgruppe **mod, size_t *size_mod, int selected_ver)
{
        int status_changenotescreen = -1;
        double new_note = -1;

        clear_display();
        print_selected_ver(*ver, *mod, *size_mod, selected_ver);
        wprintf(L"%ls%lsAchtung:%ls Möchten Sie die Note wirklich ändern?\n", TXT_RED, TXT_INVERSE, END_STYLE);
        wprintf(L"\nNeue Note eingeben     [n]        Note auf \"ausstehend\" setzen    [a]");
        wprintf(L"\nAbbruch                [b]\n\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        status_changenotescreen = read_command(INPUT_NOTE_OR_AUSSTEHEND, SIZE_INPUT_NOTE_OR_AUSSTEHEND);
        STANDARD_ERROR_HANDLING(status_changenotescreen);
        switch (status_changenotescreen) {
                case L'n':
                        clear_display();
                        print_selected_ver(*ver, *mod, *size_mod, selected_ver);
                        wprintf(L"\n\n%ls%lsNeue Note:%ls\n%ls>>>%ls ", TXT_INVERSE, TXT_RED, END_STYLE, TXT_INVERSE, END_STYLE);
                        
                        // Neue Note einlesen
                        status_changenotescreen = read_note(&new_note);
                        STANDARD_ERROR_HANDLING(status_changenotescreen);
                        (*ver)[selected_ver].note = new_note; 
                        if (new_note <= 4.0) {
                                (*ver)[selected_ver].state = Bestanden;
                        } else {
                                (*ver)[selected_ver].state = NichtBestanden;
                        }
                        return CHANGES_SAVED;
                case L'a':
                        (*ver)[selected_ver].state = Ausstehend;
                        return CHANGES_SAVED;
                case L'b':
                        return VALID_USER_INPUT;
        }
        return INVALID_FUNCTION_INPUT;

}


void print_notechangedscreen(void)
{
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                            %lsDie Note wurde geändert!%ls                                  │\n", TXT_GREEN, END_STYLE);
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────┘\n\n\n");

}


int change_modscreen(struct Veranstaltung **ver, struct Modulgruppe **mod, size_t *size_mod, int selected_ver)
{
        int status_changemodscreen = -1;
        int new_mod = -1;

        clear_display();
        print_selected_ver(*ver, *mod, *size_mod, selected_ver);
        wprintf(L"%ls%lsAchtung:%ls Möchten Sie Modulgruppe wirklich ändern?\n", TXT_RED, TXT_INVERSE, END_STYLE);
        wprintf(L"\nNeue Modulgruppe wählen          [n]");
        wprintf(L"\nAbbruch                          [b]\n\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
        status_changemodscreen = read_command(INPUT_MOD_OR_CANCEL, SIZE_INPUT_MOD_OR_CANCEL);
        STANDARD_ERROR_HANDLING(status_changemodscreen);
        switch (status_changemodscreen) {
                case L'n':
                        clear_display();
                        print_selected_ver(*ver, *mod, *size_mod, selected_ver);
                                               

                        // Vorhandene Modulgruppen anzeigen
                        sort_mod_by_alpha(*mod, *size_mod);
                        wprintf(L"\n\n%lsVorhandene Modulgruppen:%ls                   \n\n", TXT_INVERSE, END_STYLE);
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


                        // Neue Modulguppe einlesen
                        wprintf(L"\n\nNummer der Modulgruppe:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                        status_changemodscreen = read_number_in_bound(0, (int) ((*size_mod) - 1), &new_mod);
                        STANDARD_ERROR_HANDLING(status_changemodscreen);
                        (*ver)[selected_ver].modulgruppenindex = new_mod;
                        return CHANGES_SAVED;
                case L'b':
                        return VALID_USER_INPUT;
        }
        return INVALID_FUNCTION_INPUT;


}


void print_modchangedscreen(void)
{
        wprintf(L"┌──────────────────────────────────────────────────────────────────────────────────────┐\n");
        wprintf(L"│                            %lsDie Modulgruppe wurde geändert!%ls                           │\n", TXT_GREEN, END_STYLE);
        wprintf(L"└──────────────────────────────────────────────────────────────────────────────────────┘\n\n\n");

}
