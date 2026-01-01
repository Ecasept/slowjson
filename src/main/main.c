#include "../frontend/test.h"
#include "../frontend/text_formatting/text_formatting.h"
#include "../frontend/user_input/user_input.h"
#include "../frontend/edit_events/edit_events.h"
#include "../data/load.h"
#include "../data/save.h"
#include "../utils/custom_error.h"
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <math.h>





 

  
  
int main() {
	setlocale(LC_ALL, "");

	// Standardvariablen
	int status = 0;
	int current_page = -1;
	int view_type = OV_BY_TIME;


	
	


	
	// Daten aus der Datei lesen -> Speicherung in 2 dynamischen Arrays (je eines für Veranstaltungen und Modulgruppen)
	struct Veranstaltung *ver = NULL;
	size_t size_ver = 0;
	struct Modulgruppe *mod = NULL;
	size_t size_mod = 0;

	print_welcomescreen();

	// Daten aus dem Speicher laden hihihi
	Result r = load_data_from_savefile(&ver, &mod, &size_ver, &size_mod);
	if (!r.success) {
		print_error(r);
	}
	




	// Erste Eingabe + Fehlerbehandlung erste Eingabe
	status = read_command(INPUT_WELC_SCR, SIZE_INPUT_WELC_SCR);
	if (status == BUFFER_ERROR) {
		print_buffer_error_screen();
		return 0;
	} else if (status == INVALID_FUNCTION_INPUT) {
		wprintf(L"%ls Programmfehler!\nBitte neu starten.%ls", TXT_RED, END_STYLE);
		return 0;
	} else if (status == INVALID_USER_INPUT) {
		status = L'h';
	}

	// Unterscheidung Eingabeoptionen
	switch (status) {
		case L'v':
			view_type = OV_BY_TIME;
			print_overviewscreen(ver, size_ver, mod, size_mod, view_type, NO_NEW_ENTRY);
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
	

		// Fehlerbehandlung bei Programmfehlern oder falscher Eingabe
		if (status == BUFFER_ERROR) {
			print_buffer_error_screen();
			return 0;
		} else if (status == INVALID_FUNCTION_INPUT) {
			wprintf(L"%ls Programmfehler!\nBitte neu starten.%ls", TXT_RED, END_STYLE);
			return 0;
		} else if (status == INVALID_USER_INPUT && current_page == CURR_PAGE_HELP_SCR) {
			print_endscreen();
			return 0;
		} else if (status == INVALID_USER_INPUT && current_page != CURR_PAGE_HELP_SCR) {
			status = L'h'; // Hilfsseite bei viermaliger falscher Eingabe aufrufen

		}


		// Je nach Eingabe die verschiedenen Screens aufrufen
		switch (status) {
			case L'v':
				print_overviewscreen(ver, size_ver, mod, size_mod, view_type, NO_NEW_ENTRY);
				current_page = CURR_PAGE_OV_SCR;
				break;
			case L'a':
				if (view_type == OV_BY_TIME) {
					view_type = OV_BY_MOD;
				} else {
					view_type = OV_BY_TIME;
				}
				print_overviewscreen(ver, size_ver, mod, size_mod, view_type, NO_NEW_ENTRY);
				current_page = CURR_PAGE_OV_SCR;
				break;
			case L'n':
				status = print_addverscreen(&ver, &size_ver, &mod, &size_mod);
				if (status == BUFFER_ERROR) {
					print_buffer_error_screen();
					return 0;
				} else if (status == MEM_ALLOC_ERROR) {
					print_memalloc_error_screen();
					return 0;
				} else if (status == INVALID_FUNCTION_INPUT) {
					wprintf(L"%ls INVALID FUNCTION INPUT %ls", TXT_RED, END_STYLE);
					return 0;
				} else if (status == INVALID_USER_INPUT) {
					current_page = CURR_PAGE_HELP_SCR;
					print_helpscreen();
				} else if (status == VALID_USER_INPUT) {
					// Daten in Datei speichern
					r = save_data_to_savefile(ver, size_ver, mod, size_mod);
					if (!r.success) {
						print_error(r);
					}
					switch (current_page) {
						case CURR_PAGE_OV_SCR:
							print_overviewscreen(ver, size_ver, mod, size_mod, view_type, NEW_ENTRY);
							break;
						case CURR_PAGE_AV_SCR:
							print_averagescreen(NEW_ENTRY, ver, size_ver);
							break;
					}
				}
				break;
			case L'm':
				status = print_addmodscreen(&mod, &size_mod);
				if (status == BUFFER_ERROR) {
					print_buffer_error_screen();
					return 0;
				} else if (status == MEM_ALLOC_ERROR) {
					print_memalloc_error_screen();
					return 0;
				} else if (status == INVALID_FUNCTION_INPUT) {
					wprintf(L"%ls INVALID FUNCTION INPUT %ls", TXT_RED, END_STYLE);
					return 0;
				} else if (status == INVALID_USER_INPUT) {
					current_page = CURR_PAGE_HELP_SCR;
					print_helpscreen();
				} else if (status == VALID_USER_INPUT) {
					// Daten in Datei speichern
					r = save_data_to_savefile(ver, size_ver, mod, size_mod);
					if (!r.success) {
						print_error(r);
					}
					switch (current_page) {
						case CURR_PAGE_OV_SCR:
							print_overviewscreen(ver, size_ver, mod, size_mod, view_type, NEW_ENTRY);
							break;
						case CURR_PAGE_AV_SCR:
							print_averagescreen(NEW_ENTRY, ver, size_ver);
							break;
					}					
				}
				break;

			case L'd':
				print_averagescreen(NO_NEW_ENTRY, ver, size_ver);
				current_page = CURR_PAGE_AV_SCR;
				break;
			case L'b':
				status = print_editver(&ver, &size_ver, &mod, &size_mod, &current_page);
				if (status == BUFFER_ERROR) {
					print_buffer_error_screen();
					return 0;
				} else if (status == MEM_ALLOC_ERROR) {
					print_memalloc_error_screen();
					return 0;
				} else if (status == INVALID_FUNCTION_INPUT) {
					wprintf(L"%ls INVALID FUNCTION INPUT %ls", TXT_RED, END_STYLE);
					return 0;
				} else if (status == INVALID_USER_INPUT) {
					current_page = CURR_PAGE_HELP_SCR;
					print_helpscreen();
				} else if (status == VALID_USER_INPUT) {
					// Daten in Datei speichern
					r = save_data_to_savefile(ver, size_ver, mod, size_mod);
					if (!r.success) {
						print_error(r);
					}
					switch (current_page) {
						case CURR_PAGE_OV_SCR:
							print_overviewscreen(ver, size_ver, mod, size_mod, view_type, NO_NEW_ENTRY);
							break;
						case CURR_PAGE_AV_SCR:
							print_averagescreen(NO_NEW_ENTRY, ver, size_ver);
							break;
						case CURR_PAGE_HELP_SCR:
							print_helpscreen();
							break;
					}					
				}
				break;

			case L'h':
				print_helpscreen();
				current_page = CURR_PAGE_HELP_SCR;
				break;
			case L'q':
				// Daten in Datei speichern
				r = save_data_to_savefile(ver, size_ver, mod, size_mod);
				if (!r.success) {
					print_error(r);
				}
				print_endscreen();
				return 0;
		}

	}
	
}
