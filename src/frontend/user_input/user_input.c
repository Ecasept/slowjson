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
                wprintf(L"\n%ls Bitte geben Sie nur einen einzigen Buchstaben ein!%ls", TXT_RED, END_STYLE);
                wprintf(L"\n%ls>>>%ls ", TXT_INVERSE, END_STYLE);
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
                        wprintf(L"%ls%lsFalsche Eingabe%ls", TXT_RED, TXT_INVERSE, END_STYLE);
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
                        wprintf(L"\n%lsFalsche Eingabe!%ls\nFolgende Zeichen sind erlaubt:\n- Deutsche Buchstaben\n- Ziffern\n- Sonstige Zeichen: Leerzeichen , ; : . - &", TXT_RED, END_STYLE);
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
        int valid_number = -1;
        int stat = -1;
        double result = -1.0;

        while (1) {
                valid_number = 1;
                
                stat = wscanf(L"%lf", &result);

                // Fehlerbehandlung bei falscher Eingabe
                if (stat == EOF) {
                        return BUFFER_ERROR;
                } else if (stat != 1) {
                        --counter;
                        if (counter == 0) {
                                return INVALID_USER_INPUT;
                        }
                        valid_number = 0;
                        wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine gültige Note eingeben%ls\n>>> ", TXT_RED, TXT_INVERSE, END_STYLE);
                }

                if (getwchar() != L'\n') {
                        if (flush() == BUFFER_ERROR) {
                                return BUFFER_ERROR;
                        }
                        if (valid_number) {
                                --counter;
                                valid_number = 0;
                                wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine gültige Note eingeben%ls\n>>> ", TXT_RED, TXT_INVERSE, END_STYLE);
                        }
                }


                if (stat == 1 && valid_number == 1) {
                        if (result < 1.0 || result > 5.0 || !(result == 1.0 || result == 1.3 || result == 1.7 || result == 2.0 || result == 2.3 || result == 2.7 || result == 3.0 || result == 3.3 || result == 3.7 || result == 4.0 || result == 4.3 || result == 4.7 || result == 5.0)) {
                                --counter;
                                valid_number = 0;
                                wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine gültige Note eingeben%ls\n>>> ", TXT_RED, TXT_INVERSE, END_STYLE);                                
                        }
                }
                

                // Abbruch bei zu vielen falschen Eingaben
                if (counter == 0) {
                        return INVALID_USER_INPUT;
                }

                
                // Bei richtiger Eingabe, Eingabe in jahr speichern
                if (valid_number) {
                        *note = result;
                        return VALID_USER_INPUT;
                }
        }


}






int read_lp(int *lp)
{
        // Fehlermeldung, wenn NULL für lp übergeben wurde
        if (lp == NULL) {
                return INVALID_FUNCTION_INPUT;
        }

        int counter = LIMIT_INPUT_ATTEMPTS;
        int valid_number = -1;
        int stat = -1;
        int result = -1;

        while (1) {
                valid_number = 1;
                
                stat = wscanf(L"%i", &result);

                // Fehlerbehandlung bei falscher Eingabe
                if (stat == EOF) {
                        return BUFFER_ERROR;
                } else if (stat != 1) {
                        --counter;
                        if (counter == 0) {
                                return INVALID_USER_INPUT;
                        }
                        valid_number = 0;
                        wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine positive ganze Zahl eingeben%ls\n%ls>>>%ls ", TXT_RED, TXT_INVERSE, END_STYLE, TXT_INVERSE, END_STYLE);
                }


                if (getwchar() != L'\n') {
                        if (flush() == BUFFER_ERROR) {
                                return BUFFER_ERROR;
                        }
                        if (valid_number) {
                                --counter;
                                valid_number = 0;
                                wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine positive ganze Zahl eingeben%ls\n%ls>>>%ls ", TXT_RED, TXT_INVERSE, END_STYLE, TXT_INVERSE, END_STYLE);
                        }
                }


                if (stat == 1 && valid_number == 1) {
                        if (result <= 0) {
                                --counter;
                                valid_number = 0;
                                wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine positive ganze Zahl eingeben%ls\n%ls>>>%ls ", TXT_RED, TXT_INVERSE, END_STYLE, TXT_INVERSE, END_STYLE);                                
                        }
                }


                // Abbruch bei zu vielen falschen Eingaben
                if (counter == 0) {
                        return INVALID_USER_INPUT;
                }

                
                // Bei richtiger Eingabe, Eingabe in lp speichern
                if (valid_number) {
                        *lp = result;
                        return VALID_USER_INPUT;
                }
        }
}






int read_jahr(int *jahr)
{
        // Fehlermeldung, wenn NULL für jahr übergeben wurde
        if (jahr == NULL) {
                return INVALID_FUNCTION_INPUT;
        }

        int counter = LIMIT_INPUT_ATTEMPTS;
        int valid_number = -1;
        int stat = -1;
        int result = -1;

        while (1) {
                valid_number = 1;
                
                stat = wscanf(L"%i", &result);

                // Fehlerbehandlung bei falscher Eingabe
                if (stat == EOF) {
                        return BUFFER_ERROR;
                } else if (stat != 1) {
                        --counter;
                        if (counter == 0) {
                                return INVALID_USER_INPUT;
                        }
                        valid_number = 0;
                        wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine positive ganze Zahl eingeben%ls\n>>> ", TXT_RED, TXT_INVERSE, END_STYLE);
                }

                if (getwchar() != L'\n') {
                        if (flush() == BUFFER_ERROR) {
                                return BUFFER_ERROR;
                        }
                        if (valid_number) {
                                --counter;
                                valid_number = 0;
                                wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine positive ganze Zahl eingeben%ls\n>>> ", TXT_RED, TXT_INVERSE, END_STYLE);
                        }
                }


                if (stat == 1 && valid_number == 1) {
                        if (result <= 0) {
                                --counter;
                                valid_number = 0;
                                wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine positive ganze Zahl eingeben%ls\n>>> ", TXT_RED, TXT_INVERSE, END_STYLE);                                
                        }
                }
                

                // Abbruch bei zu vielen falschen Eingaben
                if (counter == 0) {
                        return INVALID_USER_INPUT;
                }

                
                // Bei richtiger Eingabe, Eingabe in jahr speichern
                if (valid_number) {
                        *jahr = result;
                        return VALID_USER_INPUT;
                }
        }



}








int read_number_in_bound(int lower_bound, int upper_bound, int *number)
{
        // Fehlermeldung, wenn NULL für number übergeben wurde oder upper_bound kleiner oder gleich lower_bound ist
        if (number == NULL || upper_bound < lower_bound) {
                return INVALID_FUNCTION_INPUT;
        }

        int counter = LIMIT_INPUT_ATTEMPTS;
        int valid_number = -1;
        int stat = -1;
        int result = -1;

        while (1) {
                valid_number = 1;
                
                stat = wscanf(L"%i", &result);

                // Fehlerbehandlung bei falscher Eingabe
                if (stat == EOF) {
                        return BUFFER_ERROR;
                } else if (stat != 1) {
                        --counter;
                        if (counter == 0) {
                                return INVALID_USER_INPUT;
                        }
                        valid_number = 0;
                        wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine positive ganze Zahl zwischen %i und %i eingeben (jeweils einschließlich)%ls\n>>> ", TXT_RED, TXT_INVERSE, lower_bound, upper_bound, END_STYLE);
                }

                if (getwchar() != L'\n') {
                        if (flush() == BUFFER_ERROR) {
                                return BUFFER_ERROR;
                        }
                        if (valid_number) {
                                --counter;
                                valid_number = 0;
                                wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine positive ganze Zahl zwischen %i und %i eingeben (jeweils einschließlich)%ls\n>>> ", TXT_RED, TXT_INVERSE, lower_bound, upper_bound, END_STYLE);
                        }
                }


                if (stat == 1 && valid_number == 1) {
                        if (result < lower_bound || result > upper_bound) {
                                --counter;
                                valid_number = 0;
                                wprintf(L"\n%ls%lsFalsche Eingabe!\nBitte eine positive ganze Zahl zwischen %i und %i eingeben (jeweils einschließlich)%ls\n>>> ", TXT_RED, TXT_INVERSE, lower_bound, upper_bound, END_STYLE);
                        }
                }
                

                // Abbruch bei zu vielen falschen Eingaben
                if (counter == 0) {
                        return INVALID_USER_INPUT;
                }

                
                // Bei richtiger Eingabe, Eingabe in number speichern
                if (valid_number) {
                        *number = result;
                        return VALID_USER_INPUT;
                }
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