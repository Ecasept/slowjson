#include <stdlib.h>
#include <stdio.h>
#include "user_input.h"
#include "../text_formatting/text_formatting.h"
#include <wchar.h>




void input_test()
{
        wprintf(L"%ls", "╠ ╡ ╢ ╣ ╤");
	wprintf(L"ÄÖÜäöüß\n>>>");
	wprintf(L"%ls", "╠ ╡ ╢ ╣ ╤ ╭");
	wprintf(L"ÄÖÜäöüß\n>>>");
	
	wchar_t v[20];
	wchar_t c = '\0';
	int i = 0;

	while((c = getwchar()) != '\n') {
		v[i] = c;
		++i;
	}
	v[i] = '\0';
	
        for (int a = 0; a < i; ++a) {
                putwchar(v[a]);
        }
       wprintf(L"\n\nAusgabe: %ls\n", v);

}




int read_command(wchar_t valid_input[], size_t size)
{
        int counter = LIMIT_INPUT_ATTEMPTS;
        int single_character = 1;
        while (1) {
                single_character = 1;
                
                if (size == 0) {
                        return INVALID_FUNCTION_INPUT;
                }
                wchar_t first_c = '\0';
                wchar_t second_c = '\0';

                first_c = getwchar();

                if (first_c == EOF) {
                        return BUFFER_ERROR;
                }

                // Prüfung ob nur einzelnes Zeichen eingegeben
                // Falls mehere Zeichen eingegeben -> counter dekrementieren
                second_c = getwchar();
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
                wprintf(L"\n%ls Bitte geben Sie nur einen einzigen Buchstaben ein!%ls\n", TXT_RED, END_STYLE);
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