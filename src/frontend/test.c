#include <stdio.h>
#include <wchar.h>
#include <stddef.h>

void printer_test()
{
        printf("TEST!!");
        printf("\n Test für commit und pullüäö");
        printf("\nEin weiterer Test\n");
       

    	printf("%s", "╠ ╡ ╢ ╣ ╤");
	printf("ÄÖÜäöüß\n>>>");
	wchar_t c;
        scanf("%lc", &c);
        while(1) {
                if (getchar() == '\n') {
                        break;
                }
        }
	printf("\nHallo: %Lc\n", c);
        return 0;

}
