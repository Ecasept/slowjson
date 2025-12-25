#include <stdio.h>


// Eingabe von Umlaute als zwei normale char

void printer_test()
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
