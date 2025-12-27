#include <stdlib.h>
#include <stdio.h>
#include "user_input.h"
#include "../text_formatting/text_formatting.h"




void input_test()
{
        printf("%s", "╠ ╡ ╢ ╣ ╤");
	printf("ÄÖÜäöüß\n>>>");
	printf("%s", "╠ ╡ ╢ ╣ ╤ ╭");
	printf("ÄÖÜäöüß\n>>>");
	
	char v[20];
	char c = '\0';
	int i = 0;

	while((c = getchar()) != '\n') {
		v[i] = c;
		++i;
	}
	v[i] = '\0';
	
        for (int a = 0; a < i; ++a) {
                putchar(v[a]);
        }
       printf("\n\nAusgabe: %s\n", v);

}




int read_command(char valid_input[], size_t size)
{
        int counter = LIMIT_INPUT_ATTEMPTS;
        int single_character = 1;
        while (1) {
                single_character = 1;
                
                if (size == 0) {
                        return INVALID_FUNCTION_INPUT;
                }
                char first_c = '\0';
                char second_c = '\0';

                first_c = getchar();

                if (first_c == EOF) {
                        return BUFFER_ERROR;
                }

                // Prüfung ob nur einzelnes Zeichen eingegeben
                // Falls mehere Zeichen eingegeben -> counter dekrementieren
                second_c = getchar();
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
                printf("\n%s Bitte geben Sie nur einen einzigen Buchstaben ein!%s\n", TXT_RED, END_STYLE);
        }
}



int flush()
{
        char c = '\0';
        while(1) {
                c = getchar();
                if (c == EOF) {
                        return BUFFER_ERROR;
                }
                if (c == '\n') {
                        return FLUSH_COMPLETE;
                }
        }
}