#include "../frontend/test.h"
#include "../json/lexer.h"
#include <stdio.h>

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

	printf("It works!\n");
	printer_test();
}
