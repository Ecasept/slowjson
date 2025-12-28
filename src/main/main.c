#include "../json/jsontest.h"
#include <stdio.h>
#include "../frontend/test.h"
#include "../frontend/text_formatting/text_formatting.h"
#include "../midend/data.h"
#include "../frontend/user_input/user_input.h"
#include <string.h>
#include <wchar.h>
#include <locale.h>




#define CURR_PAGE_OV_SCR 1
#define CURR_PAGE_AV_SCR 2
#define CURR_PAGE_HELP_SCR 3

#define OV_BY_TIME 1
#define OV_BY_MOD 2

  
  
int main() {
	setlocale(LC_ALL, "");	
	jsontest();
	
	// Standardvariablen
	int status = 0;
	int current_page = -1;
	int view_type = -1;


	
	// Daten aus der Datei lesen -> Speicherung in 2 dynamischen Arrays (je eines für Veranstaltungen und Modulgruppen)
	struct Veranstaltung *ver = NULL;
	size_t size_ver = 0;
	struct Modulgruppe *mod= NULL;
	size_t size_mod = 0;
	get_test_data(&ver, &size_ver, &mod, &size_mod);
	



	print_welcomescreen();

	// Erste Eingabe + Fehlerbehandlung erste Eingabe
	status = read_command(INPUT_WELC_SCR, SIZE_INPUT_WELC_SCR);
	if (status == BUFFER_ERROR) {
		wprintf(L"%ls BUFFER ERROR %ls", TXT_RED, END_STYLE);
		return 0;
	} else if (status == INVALID_FUNCTION_INPUT) {
		wprintf(L"%ls Programmfehler!\nBitte neu starten.%ls", TXT_RED, END_STYLE);
		return 0;
	} else if (status == INVALID_USER_INPUT) {
		wprintf(L"\n%ls%ls Viermal falsche Eingabe. Bitte neu starten\n%ls", TXT_INVERSE, TXT_RED, END_STYLE);
		return 0;
	}

	// Unterscheidung Eingabeoptionen
	switch (status) {
		case L'v':
			view_type = OV_BY_TIME;
			print_overviewscreen(ver, size_ver, mod, size_mod, view_type);
			current_page = CURR_PAGE_OV_SCR;
			break;
		case L'h':
			print_helpscreen();
			current_page = CURR_PAGE_HELP_SCR;
			break;
		case L'q':
			print_endscreen();
			return 0;
	}


	// Endlossschleife Benutzereingaben
	while (1) {
		// Eingabeoptionen je nach aktueller Seite
		status = L'\0';
		switch (current_page) {
			case CURR_PAGE_OV_SCR:
				status = read_command(INPUT_OVERVIEW_SCR, SIZE_INPUT_OVERVIEW_SCR);
				break;
			case CURR_PAGE_AV_SCR:
				status = read_command(INPUT_AVERAGE_SCR, SIZE_INPUT_AVERAGE_SCR);
				break;
			case CURR_PAGE_HELP_SCR:
				status = read_command(INPUT_HELP_SCR, SIZE_INPUT_HELP_SCR);
				break;
		}
	

	
		// Fehlerbehandlung bei Fehlern oder falscher Eingabe
		if (status == BUFFER_ERROR) {
			wprintf(L"%ls BUFFER ERROR %ls", TXT_RED, END_STYLE);
			return 0;
		} else if (status == INVALID_FUNCTION_INPUT) {
			wprintf(L"%ls Programmfehler!\nBitte neu starten.%ls", TXT_RED, END_STYLE);
			return 0;
		} else if (status == INVALID_USER_INPUT) {
			wprintf(L"\n%ls%ls Viermal falsche Eingabe. Bitte neu starten\n%ls", TXT_INVERSE, TXT_RED, END_STYLE);
			return 0;
		}


		// Je nach Eingabe die verschiedenen Screens aufrufen
		switch (status) {
			case L'v':
				view_type = OV_BY_TIME;
				print_overviewscreen(ver, size_ver, mod, size_mod, view_type);
				current_page = CURR_PAGE_OV_SCR;
				break;
			case L'a':
				if (view_type == OV_BY_TIME) {
					view_type = OV_BY_MOD;
				} else {
					view_type = OV_BY_TIME;
				}
				print_overviewscreen(ver, size_ver, mod, size_mod, view_type);
				current_page = CURR_PAGE_OV_SCR;
				break;

			case L'd':
				print_averagescreen();
				current_page = CURR_PAGE_AV_SCR;
				break;
			case L'h':
				print_helpscreen();
				current_page = CURR_PAGE_HELP_SCR;
				break;
			case L'q':
				print_endscreen();
				return 0;
		}

	}
	
}
