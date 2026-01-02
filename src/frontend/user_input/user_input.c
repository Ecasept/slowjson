#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "user_input.h"
#include "../text_formatting/text_formatting.h"
#include <wchar.h>
#include <wctype.h>






int read_command(wchar_t valid_input[], size_t size)
{
        // Prüfung ob valid_input[] nicht NULL und size nicht 0
        if (size == 0 || valid_input == NULL) {
                        return INVALID_FUNCTION_INPUT;
        }

        int counter = LIMIT_INPUT_ATTEMPTS;
        int single_character = 1;
        while (1) {
                single_character = 1;
                
                
                wchar_t first_c = '\0';
                wchar_t second_c = '\n';

                first_c = getwchar();

                if (first_c == EOF) {
                        return BUFFER_ERROR;
                } 
                if (first_c != '\n') {
                        second_c = getwchar();

                } 

                // Prüfung ob nur einzelnes Zeichen eingegeben
                // Falls mehere Zeichen eingegeben -> counter dekrementieren

                if (second_c == EOF) {
                        return BUFFER_ERROR;
                } else if (second_c != '\n') {
                        if (flush() == BUFFER_ERROR) {
                                return BUFFER_ERROR;
                        } else {
                                single_character = 0;
                        }
                }

                // Prüfung ob first_c in valid_input[] enthalten ist
                int valid = 0;
                for (size_t i = 0; i < size; ++i) {
                        if (valid_input[i] == first_c)
                                valid = 1;
                }

                if (valid == 1 && single_character == 1)
                        return first_c;

                --counter;
                if (counter == 0) {
                        return INVALID_USER_INPUT;
                }
                print_wrong_command_screen(valid_input, size);
        }
}





int read_string(wchar_t **s)
{
        // Fehlermeldung, wenn für s keine Adresse übergeben wird
        if (s == NULL) {
                return INVALID_FUNCTION_INPUT;
        }

        int counter = LIMIT_INPUT_ATTEMPTS;
        int valid_character = -1;

        wchar_t c = L'\0';
        int i = 0;
        size_t size = 1;
        wchar_t *p_1 = NULL;
        wchar_t *p_2 = NULL;
        
        // Zeichenweise Eingabe auslesen
        while(1) {
                c = getwchar();
                if (c == EOF) {
                        return BUFFER_ERROR;
                }
                valid_character = 1;

                // Fehlermeldung, wenn nichts eingegeben wurde
                if (size == 1 && c == L'\n') {
                        valid_character = 0;
                        --counter;
                        print_wrong_string_screen();
                }


                // Dynamisch reservierten Speicher um 1 vergrößern
                p_2 = realloc(p_1, size * sizeof(wchar_t));
                if (p_2 == NULL) {
                        free(p_1);
                        return MEM_ALLOC_ERROR;
                }
                p_1 = p_2;
                p_2 = NULL;

                
                if (c == L'\n' && valid_character == 1) {
                        p_1[i] = L'\0';
                        *s = p_1;
                        return VALID_USER_INPUT;                        
                }

                // Prüfung ob eingegebenes Zeichen Buchstabe, Zahl oder Leerzeichen ist
                if (!(iswalnum(c) || iswpunct(c) || iswspace(c))) {
                        print_wrong_string_screen();
                        --counter;
                        valid_character = 0;
                        size = 1;
                        i = 0;
                        free(p_1);
                        if (flush() == BUFFER_ERROR) {
                                return BUFFER_ERROR;
                        }
                }

                // Falls Zeichen gültig -> In Array speichern
                if (valid_character) {
                        p_1[i] = c;
                        ++size;
                        ++i;
                }

                // Abbruch bei zu vielen falschen Eingabeversuchen
                if (counter == 0) {
                        return INVALID_USER_INPUT;
                }
        }

}







int read_note(double *note)
{
        // Fehlermeldung, wenn NULL für note übergeben wurde
        if (note == NULL) {
                return INVALID_FUNCTION_INPUT;
        }

        int counter = LIMIT_INPUT_ATTEMPTS;
        int stat = -1;
        double result = -1.0;
        wchar_t array_number[4] = {0};
        int i = 0;

        while (1) {

                while ((stat = getwchar())) {
                        if (stat == L'\n' && i == 0) {
                                i = 0;
                                --counter;
                                wprintf(L"\n\n");
                                wprintf(L"┌────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                   %ls%lsFALSCHE EINGABE!%ls                 │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│        Bitte geben Sie eine gültige Note ein.      │\n");
                                wprintf(L"└────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nNote eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        return INVALID_USER_INPUT;
                                }
                                continue;
                        }

                        if (stat == L'\n') {
                                break;
                        }


                        if (i == 3 && stat != '\n') {
                                if (flush() == BUFFER_ERROR) {
                                        return BUFFER_ERROR;
                                }
                                --counter;
                                wprintf(L"\n\n");
                                wprintf(L"┌────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                   %ls%lsFALSCHE EINGABE!%ls                 │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│        Bitte geben Sie eine gültige Note ein.      │\n");
                                wprintf(L"└────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nNote eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        return INVALID_USER_INPUT;
                                }
                                i = 0;
                                continue;                        
                        }

                        


                        array_number[i] = stat;
                        ++i;
                }

                array_number[i] = L'\0';


                result = wcstod(array_number, NULL);


                if (result < 1.0 || result > 5.0 || !(result == 1.0 || result == 1.3 || result == 1.7 || result == 2.0 || result == 2.3 || result == 2.7 || result == 3.0 || result == 3.3 || result == 3.7 || result == 4.0 || result == 4.3 || result == 4.7 || result == 5.0)) {
                        --counter;
                        if (counter == 0) {
                                return INVALID_USER_INPUT;
                        }
                        wprintf(L"\n\n");
                        wprintf(L"┌────────────────────────────────────────────────────┐\n");
                        wprintf(L"│                   %ls%lsFALSCHE EINGABE!%ls                 │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                        wprintf(L"│        Bitte geben Sie eine gültige Note ein.      │\n");
                        wprintf(L"└────────────────────────────────────────────────────┘"); 
                        wprintf(L"\n\nNote eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);           
                        i = 0;
                        continue;                
                }
        

                

                
                // Bei richtiger Eingabe, Eingabe in note speichern
                *note = result;
                return VALID_USER_INPUT;
        }


}






int read_lp(int *lp)
{
        // Fehlermeldung, wenn NULL für lp übergeben wurde
        if (lp == NULL) {
                return INVALID_FUNCTION_INPUT;
        }

        int counter = LIMIT_INPUT_ATTEMPTS;
        int stat = -1;
        int result = -1;
        wchar_t array_number[4] = {0};
        int i = 0;

        while (1) {

                while ((stat = getwchar())) {
                        if (stat == L'\n' && i == 0) {
                                i = 0;
                                --counter;
                                wprintf(L"\n\n");
                                wprintf(L"┌──────────────────────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                       %ls%lsFALSCHE EINGABE!%ls                               │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│      Bitte geben Sie eine gültige Anzahl an Leistungspunkten ein.    │\n");
                                wprintf(L"└──────────────────────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nLP eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        return INVALID_USER_INPUT;
                                }
                                continue;
                        }

                        if (stat == L'\n') {
                                break;
                        }


                        if (i == 3 && stat != '\n') {
                                if (flush() == BUFFER_ERROR) {
                                        return BUFFER_ERROR;
                                }
                                --counter;
                                wprintf(L"\n\n");
                                wprintf(L"┌──────────────────────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                       %ls%lsFALSCHE EINGABE!%ls                               │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│      Bitte geben Sie eine gültige Anzahl an Leistungspunkten ein.    │\n");
                                wprintf(L"└──────────────────────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nLP eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        return INVALID_USER_INPUT;
                                }
                                i = 0;
                                continue;                        
                        }

                        


                        array_number[i] = stat;
                        ++i;
                }

                array_number[i] = L'\0';


                result = wcstol(array_number, NULL, 10);


                if (result < 1) {
                        --counter;
                        if (counter == 0) {
                                return INVALID_USER_INPUT;
                        }
                        wprintf(L"\n\n");
                        wprintf(L"┌──────────────────────────────────────────────────────────────────────┐\n");
                        wprintf(L"│                       %ls%lsFALSCHE EINGABE!%ls                               │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                        wprintf(L"│      Bitte geben Sie eine gültige Anzahl an Leistungspunkten ein.    │\n");
                        wprintf(L"└──────────────────────────────────────────────────────────────────────┘");
                        wprintf(L"\n\nLP eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);  
                        i = 0;
                        continue;                
                }
        
                
                // Bei richtiger Eingabe, Eingabe in lp speichern
                *lp = result;
                return VALID_USER_INPUT;
        }    
}






int read_jahr(int *jahr)
{
        // Fehlermeldung, wenn NULL für jahr übergeben wurde
        if (jahr == NULL) {
                return INVALID_FUNCTION_INPUT;
        }

        int counter = LIMIT_INPUT_ATTEMPTS;
        int stat = -1;
        int result = -1;
        wchar_t array_number[6] = {0};
        int i = 0;

        while (1) {

                while ((stat = getwchar())) {
                        if (stat == L'\n' && i == 0) {
                                i = 0;
                                --counter;
                                wprintf(L"\n\n");
                                wprintf(L"┌──────────────────────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                          %ls%lsFALSCHE EINGABE!%ls                            │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│               Bitte geben Sie eine gültige Jahreszahl ein.           │\n");
                                wprintf(L"└──────────────────────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nJahreszahl eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        return INVALID_USER_INPUT;
                                }
                                continue;
                        }

                        if (stat == L'\n') {
                                break;
                        }


                        if (i == 5 && stat != '\n') {
                                if (flush() == BUFFER_ERROR) {
                                        return BUFFER_ERROR;
                                }
                                --counter;
                                wprintf(L"\n\n");
                                wprintf(L"┌──────────────────────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                          %ls%lsFALSCHE EINGABE!%ls                            │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│               Bitte geben Sie eine gültige Jahreszahl ein.           │\n");
                                wprintf(L"└──────────────────────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nJahreszahl eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        return INVALID_USER_INPUT;
                                }
                                i = 0;
                                continue;                        
                        }


                        if (!iswdigit(stat)) {
                                if (flush() == BUFFER_ERROR) {
                                        return BUFFER_ERROR;
                                }
                                --counter;
                                wprintf(L"\n\n");
                                wprintf(L"┌──────────────────────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                          %ls%lsFALSCHE EINGABE!%ls                            │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│               Bitte geben Sie eine gültige Jahreszahl ein.           │\n");
                                wprintf(L"└──────────────────────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nJahreszahl eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        return INVALID_USER_INPUT;
                                }
                                i = 0;
                                continue;                        
                        }

                        


                        array_number[i] = stat;
                        ++i;
                }

                array_number[i] = L'\0';


                result = wcstol(array_number, NULL, 10);


                if (result < 1) {
                        --counter;
                        if (counter == 0) {
                                return INVALID_USER_INPUT;
                        }
                        wprintf(L"\n\n");
                        wprintf(L"┌──────────────────────────────────────────────────────────────────────┐\n");
                        wprintf(L"│                          %ls%lsFALSCHE EINGABE!%ls                            │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                        wprintf(L"│               Bitte geben Sie eine gültige Jahreszahl ein.           │\n");
                        wprintf(L"└──────────────────────────────────────────────────────────────────────┘");
                        wprintf(L"\n\nJahreszahl eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                        i = 0;
                        continue;                
                }
        
                
                // Bei richtiger Eingabe, Eingabe in jahr speichern
                *jahr = result;
                return VALID_USER_INPUT;
        }    
}








int read_number_in_bound(int lower_bound, int upper_bound, int *number)
{
        // Fehlermeldung, wenn NULL für number übergeben wurde oder upper_bound kleiner oder lower_bound ist
        if (number == NULL || upper_bound < lower_bound) {
                return INVALID_FUNCTION_INPUT;
        }

        int counter = LIMIT_INPUT_ATTEMPTS;
        int stat = -1;
        int result = -1;
        wchar_t *array_number = NULL;
        int i = 0;

        while (1) {

                while ((stat = getwchar())) {
                        wchar_t *array_number_temp = realloc(array_number, ((i + 1) * sizeof(wchar_t)));
                        if (array_number_temp == NULL) {
                                free(array_number);
                                return MEM_ALLOC_ERROR;
                        } else {
                                array_number = array_number_temp;
                        }
                        if (stat == L'\n' && i == 0) {
                                i = 0;
                                --counter;
                                wprintf(L"\n");
                                wprintf(L"┌─────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                                         %ls%lsFALSCHE EINGABE!%ls                                        │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│              Bitte geben Sie eine Zahl zwischen %i und %i ein (jeweils einschließlich).           │\n", lower_bound, upper_bound);
                                wprintf(L"└─────────────────────────────────────────────────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nGültige Zahl eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        free(array_number);
                                        return INVALID_USER_INPUT;
                                }
                                continue;
                        }

                        if (stat == L'\n') {
                                break;
                        }


                        if (i == 5 && stat != '\n') {
                                if (flush() == BUFFER_ERROR) {
                                        free(array_number);
                                        return BUFFER_ERROR;
                                }
                                --counter;
                                wprintf(L"\n");
                                wprintf(L"┌─────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                                         %ls%lsFALSCHE EINGABE!%ls                                        │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│              Bitte geben Sie eine Zahl zwischen %i und %i ein (jeweils einschließlich).           │\n", lower_bound, upper_bound);
                                wprintf(L"└─────────────────────────────────────────────────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nGültige Zahl eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        free(array_number);
                                        return INVALID_USER_INPUT;
                                }
                                i = 0;
                                continue;                        
                        }


                        if (!iswdigit(stat)) {
                                if (flush() == BUFFER_ERROR) {
                                        free(array_number);
                                        return BUFFER_ERROR;
                                }
                                --counter;
                                wprintf(L"\n");
                                wprintf(L"┌─────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
                                wprintf(L"│                                         %ls%lsFALSCHE EINGABE!%ls                                        │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                                wprintf(L"│              Bitte geben Sie eine Zahl zwischen %i und %i ein (jeweils einschließlich).           │\n", lower_bound, upper_bound);
                                wprintf(L"└─────────────────────────────────────────────────────────────────────────────────────────────────┘");
                                wprintf(L"\n\nGültige Zahl eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                                if (counter == 0) {
                                        free(array_number);
                                        return INVALID_USER_INPUT;
                                }
                                i = 0;
                                continue;                        
                        }

                        


                        array_number[i] = stat;
                        ++i;
                }

                array_number[i] = L'\0';


                result = wcstol(array_number, NULL, 10);


                if (result < lower_bound || result > upper_bound) {
                        --counter;
                        if (counter == 0) {
                                free(array_number);
                                return INVALID_USER_INPUT;
                        }
                        wprintf(L"\n");
                        wprintf(L"┌─────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
                        wprintf(L"│                                         %ls%lsFALSCHE EINGABE!%ls                                        │\n", TXT_INVERSE, TXT_RED, END_STYLE);
                        wprintf(L"│              Bitte geben Sie eine Zahl zwischen %i und %i ein (jeweils einschließlich).           │\n", lower_bound, upper_bound);
                        wprintf(L"└─────────────────────────────────────────────────────────────────────────────────────────────────┘");
                        wprintf(L"\n\nGültige Zahl eingeben:\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
                        i = 0;
                        continue;                
                }
        
                
                // Bei richtiger Eingabe, Eingabe in number speichern
                *number = result;
                free(array_number);
                return VALID_USER_INPUT;    
        }
}























int flush()
{
        wchar_t c = L'\0';
        while(1) {
                c = getwchar();
                if (c == EOF) {
                        return BUFFER_ERROR;
                }
                if (c == L'\n') {
                        return FLUSH_COMPLETE;
                }
        }
}
