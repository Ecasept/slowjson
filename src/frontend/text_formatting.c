#include "text_formatting.h"


void print_welcomescreen()
{
        printf("┌───────────────────────────────────────────────────────┐\n");
        printf("│                                                       │\n");
        printf("│%s     |  |  |  |    |/  /\\  |\\/|  |\\/|  |=  |\\ |  │     %s│\n", GREEN, END_FORMAT);
        printf("│%s     |/\\|  |  |__  |\\  \\/  |  |  |  |  |=  | \\|  .     %s│\n", GREEN, END_FORMAT);
        printf("│                                                       │\n");
        printf("│Dies ist ihr persönlicher Studienplaner                │\n");
        printf("│                                                       │\n");
        printf("│%sOptionen%s                        %sTaste%s                  │\n", UNDERLINED, END_FORMAT, UNDERLINED, END_FORMAT);
        printf("│Veranstaltungsübersicht           v                    │\n");
        printf("│Programm beenden                  q                    │\n");
        printf("│                                                       │\n");
        printf("└───────────────────────────────────────────────────────┘");
        printf("\n>>>⛵\n");
}