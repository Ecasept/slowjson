#include "../json/lexer.h"
#include <stdio.h>
#include "../frontend/test.h"
#include "../frontend/text_formatting/text_formatting.h"
#include "../midend/data.h"
#include "../frontend/user_input/user_input.h"


#define CURR_PAGE_OV_SCR 1
#define CURR_PAGE_AV_SCR 2
#define CURR_PAGE_HELP_SCR 3


static unsigned lcg(unsigned *s) {
	*s = (*s) * 1103515245u + 12345u;
	return *s;
}
	

int main() {
	
	string str;
	string_new(&str,
			   "{ \"name\": \"John Doe\", \"age\": 30, \"is_student\": false, "
			   "\"courses\": [\"Math\", \"Science\", \"History\"], "
			   "\"address\": { \"street\": \"123 Main St\", \"city\": "
			   "\"Anytown\" } }");

	Lexer lexer;
	lexer_init(&lexer, &str);

	JSONToken token;
	Result r;
	while (1) {
		r = lexer_next_token(&lexer, &token);
		if (!r.success) {
			print_error(r);
			error_free(r);
			break;
		}
		if (token.type == JSONTok_EOF) {
			lexer_free_token(&token);
			error_free(r);
			break;
		}
		char *value;
		string_to_cstr(&token.value, &value);
		printf("Token: %s \"%s\" (line %zu, column %zu)\n",
			   tk_as_str(token.type), value ? value : "(null)", token.line,
			   token.column);
		free(value);

		lexer_free_token(&token);
	}

	string_free(&str);
	

	

	




	
	// Standardvariablen
	int status = 0;
	int current_page = -1;
	int view_type = 1;


	
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
		printf("%s BUFFER ERROR %s", TXT_RED, END_STYLE);
		return 0;
	} else if (status == INVALID_FUNCTION_INPUT) {
		printf("%s Programmfehler!\nBitte neu starten.%s", TXT_RED, END_STYLE);
		return 0;
	} else if (status == INVALID_USER_INPUT) {
		printf("\n%s%s Viermal falsche Eingabe. Bitte neu starten\n%s", TXT_INVERSE, TXT_RED, END_STYLE);
		return 0;
	}

	// Unterscheidung Eingabeoptionen
	switch (status) {
		case 'v':
			print_overviewscreen(ver, size_ver, mod, size_mod, view_type);
			current_page = CURR_PAGE_OV_SCR;
			break;
		case 'h':
			print_helpscreen();
			current_page = CURR_PAGE_HELP_SCR;
			break;
		case 'q':
			print_endscreen();
			return 0;
	}


	// Endlossschleife Benutzereingaben
	while (1) {
		// Eingabeoptionen je nach aktueller Seite
		status = '\0';
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
			printf("%s BUFFER ERROR %s", TXT_RED, END_STYLE);
			return 0;
		} else if (status == INVALID_FUNCTION_INPUT) {
			printf("%s Programmfehler!\nBitte neu starten.%s", TXT_RED, END_STYLE);
			return 0;
		} else if (status == INVALID_USER_INPUT) {
			printf("\n%s%s Viermal falsche Eingabe. Bitte neu starten\n%s", TXT_INVERSE, TXT_RED, END_STYLE);
			return 0;
		}


		// Je nach Eingabe die verschiedenen Screens aufrufen
		switch (status) {
			case 'v':
				print_overviewscreen(ver, size_ver, mod, size_mod, view_type);
				current_page = CURR_PAGE_OV_SCR;
				break;
			case 'd':
				print_averagescreen();
				current_page = CURR_PAGE_AV_SCR;
				break;
			case 'h':
				print_helpscreen();
				current_page = CURR_PAGE_HELP_SCR;
				break;
			case 'q':
				print_endscreen();
				return 0;
		}

	}
	
}
