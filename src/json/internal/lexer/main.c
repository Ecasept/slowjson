#include "../../utils/unicode/utf8.h"
#include "../../utils/unicode/utf16.h"
#include <ctype.h>
#include "number.h"
#include "utils.h"

// ==== Relevant Specifications ====
// JSON Website (with syntax diagram):
// https://www.json.org/json-en.html
// RFC JSON Specification:
// https://datatracker.ietf.org/doc/html/rfc8259
// ECMA JSON Standard:
// https://ecma-international.org/publications-and-standards/standards/ecma-404/
// UTF-8 Website with resources:
// https://www.utf8.com/
// Unicode Standard:
// https://www.unicode.org/versions/Unicode17.0.0/UnicodeStandard-17.0.pdf
// UTF-8 Standard:
// https://www.ietf.org/rfc/rfc3629.txt

// ==== Forward Declarations ====
static Result lexer_lex_structural(Lexer *lexer, JSONToken *token);
static Result lexer_lex_whitespace(Lexer *lexer, JSONToken *token);
static Result lexer_lex_string(Lexer *lexer, JSONToken *token);
static Result lexer_lex_literal(Lexer *lexer, JSONToken *token);
static Result lexer_test_literal(Lexer *lexer, string *string, bool *res);

const char *JSONTokenTypeStrings[] = {FOREACH_TOKEN(DECLARE_TOKEN_STRING)};

void lexer_init(Lexer *lexer, const string *source, ParserConfig config) {
	lexer->source = source;
	lexer->position = 0;
	lexer->line = 0;
	lexer->column = 0;
	lexer->config = config;
}



Result lexer_next_token(Lexer *lexer, JSONToken *token) {
	token->column = lexer->column;
	token->line = lexer->line;
	token->type = JSONTok_Unknown;


	UCP current_char;
	Result r = lexer_peek(lexer, &current_char);
	if (r.type == ELexerEOF) {
		token->type = JSONTok_EOF;
		string_new(&token->value, "");
		error_free(r);
		return new_success();
	}
	check(r);

	switch (current_char) {
	case '{':
	case '}':
	case '[':
	case ']':
	case ':':
	case ',':
		return lexer_lex_structural(lexer, token);
	case '\n':
	case '\r':
	case '\t':
	case ' ':
		return lexer_lex_whitespace(lexer, token);
	case '"':
		check(lexer_consume(lexer, &current_char));
		return lexer_lex_string(lexer, token);
	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return lexer_lex_number(lexer, token);
	default:
		if (isalpha(current_char)) {
			return lexer_lex_literal(lexer, token);
		} else {
			return new_errorf("Unexpected character \"%c\" at line %zu, column %zu (expected start of new token)",
							  ELexerSyntaxError, current_char, lexer->line,
							  lexer->column);
		}
	}
}

static Result lexer_lex_structural(Lexer *lexer, JSONToken *token) {
	// Implementation for lexing structural characters
	UCP current_char;
	check(lexer_consume(lexer, &current_char));

	switch (current_char) {
	case '{':
		token->type = JSONTok_LBrace;
		break;
	case '}':
		token->type = JSONTok_RBrace;
		break;
	case '[':
		token->type = JSONTok_LBracket;
		break;
	case ']':
		token->type = JSONTok_RBracket;
		break;
	case ':':
		token->type = JSONTok_Colon;
		break;
	case ',':
		token->type = JSONTok_Comma;
		break;
	default:
		return new_errorf("Unexpected character \"%c\" at line %zu, column %zu (expected start of structural token)",
						  ELexerSyntaxError, current_char, lexer->line,
						  lexer->column - 1);
	}

	string_new(&token->value, "");
	string_append_uchar(&token->value, current_char);
	return new_success();
}

static Result lexer_lex_whitespace(Lexer *lexer, JSONToken *token) {
	UCP current_char;

	string whitespace;
	string_new(&whitespace, "");

	while (1) {
		Result r = lexer_peek(lexer, &current_char);
		if (r.type == ELexerEOF) {
			// Reached end of file, return what we have
			token->type = JSONTok_Whitespace;
			token->value = whitespace;
			error_free(r);
			return new_success();
		} else if (!r.success) {
			string_free(&whitespace);
			return r;
		}

		switch (current_char) {
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			check(lexer_consume(lexer, &current_char));
			if (current_char == '\n') {
				lexer->column = 0;
				lexer->line++;
			}

			// Append whitespace character to current whitespace
			string_append_uchar(&whitespace, current_char);
			break;
		default:
			// Found non-whitespace character, which means we finished parsing
			// all of the whitespace
			token->type = JSONTok_Whitespace;
			token->value = whitespace;
			return new_success();
		}
	}
}

static Result lexer_test_literal(Lexer *lexer, string *expect, bool *res) {
	string substr;
	Result r = string_substr(lexer->source, &substr, lexer->position,
							 expect->arr.length);
	if (r.type == ESubstrOutOfRange) {
		*res = false;
		error_free(r);
		return new_success();
	} else if (!r.success) {
		return r;
	}

	string_eq(&substr, expect, res);

	string_free(&substr);

	return new_success();
}

static Result lexer_lex_specific_literal(Lexer *lexer, JSONToken *token,
								  string *literal, bool *matched,
								  JSONTokenType type) {
	bool matches;
	check(lexer_test_literal(lexer, literal, &matches));
	if (matches) {
		token->type = type;
		token->value = *literal;
		lexer->position += literal->arr.length;
		lexer->column += literal->arr.length;
		*matched = true;
		return new_success();
	}
	*matched = false;
	return new_success();
}

static Result lexer_lex_literal(Lexer *lexer, JSONToken *token) {
	string true_str;
	string_new(&true_str, "true");
	bool matches;
	check(lexer_lex_specific_literal(lexer, token, &true_str, &matches,
								   JSONTok_True));
	if (matches) {
		return new_success();
	}
	string_free(&true_str);

	string false_str;
	string_new(&false_str, "false");
	check(lexer_lex_specific_literal(lexer, token, &false_str, &matches,
								   JSONTok_False));
	if (matches) {
		return new_success();
	}
	string_free(&false_str);

	string null_str;
	string_new(&null_str, "null");
	check(lexer_lex_specific_literal(lexer, token, &null_str, &matches,
								   JSONTok_Null));
	if (matches) {
		return new_success();
	}
	string_free(&null_str);

	return new_errorf("Unexpected literal at line %zu, column %zu",
					  ELexerSyntaxError, lexer->line, lexer->column);
}

static Result lexer_lex_4_hex(Lexer *lexer, UCP *out) {
	UCP vals[4];
	for (int i = 0; i < 4; i++) {
		UCP c;
		check(lexer_consume(lexer, &c));
		if (c >= 'A' && c <= 'F') {
			vals[i] = c - 'A' + 10;
		} else if (c >= 'a' && c <= 'f') {
			vals[i] = c - 'a' + 10;
		} else if (c >= '0' && c <= '9') {
			vals[i] = c - '0';
		} else {
			return new_errorf("Invalid hex digit \"%c\" at line %zu, column %zu",
							  ELexerSyntaxError, c, lexer->line,
							  lexer->column - 1);
		}
	}
	*out = (vals[0] << 12) | (vals[1] << 8) | (vals[2] << 4) | vals[3];
	return new_success();
}
static Result lexer_lex_unicode_literal(Lexer *lexer, UCP *out) {
	UCP cp;
	check(lexer_lex_4_hex(lexer, &cp));
	if (is_high_surrogate(cp)) {
		UCP backslash;
		UCP u;
		Result r1 = lexer_consume(lexer, &backslash);
		Result r2 = lexer_consume(lexer, &u);
		if (r1.type == ELexerEOF || r2.type == ELexerEOF) {
			return new_error(
				"Unexpected end of file after high surrogate in unicode escape",
				ELexerSyntaxError);
		} else if (!r1.success) {
			return r1;
		} else if (!r2.success) {
			return r2;
		}
		if (backslash != '\\' || u != 'u') {
			return new_errorf("Expected \\u after high surrogate, got \"%c%c\" at line %zu, column %zu",
							  ELexerSyntaxError, backslash, u, lexer->line,
							  lexer->column-2);
		}
		UCP low_surrogate;
		check(lexer_lex_4_hex(lexer, &low_surrogate));
		if (!is_low_surrogate(low_surrogate)) {
			return new_errorf(
				"Expected low surrogate after high surrogate, got U+%04X at line %zu, column %zu",
				ELexerSyntaxError, low_surrogate, lexer->line, lexer->column-2);
		}
		cp = decode_surrogate_pair(cp, low_surrogate);
	} else if (is_low_surrogate(cp)) {
		return new_errorf(
			"Unexpected low surrogate U+%04X without preceding high surrogate at line %zu, column %zu",
			ELexerSyntaxError, cp, lexer->line, lexer->column - 6);
	}
	*out = cp;
	return new_success();
}

static Result lexer_lex_string(Lexer *lexer, JSONToken *token) {
	UCP chr;

	string value;
	string_new(&value, "");

	bool is_escaped = false;

	while (1) {
		Result r = lexer_consume(lexer, &chr);
		if (!r.success) {
			string_free(&value);
			return r;
		}

		if (chr <= 0x1F) {
			string_free(&value);
			return new_errorf(
				"Encountered control character with value %u in string at line %zu, column %zu",
				ELexerSyntaxError, chr, lexer->line, lexer->column - 1);
		}

		if (is_escaped) {
			switch (chr) {
			case '"':
				string_append_uchar(&value, '"');
				break;
			case '\\':
				string_append_uchar(&value, '\\');
				break;
			case '/':
				string_append_uchar(&value, '/');
				break;
			case 'b':
				// backspace
				string_append_uchar(&value, '\b');
				break;
			case 'f':
				// form feed
				string_append_uchar(&value, '\f');
				break;
			case 'n':
				// line feed
				string_append_uchar(&value, '\n');
				break;
			case 'r':
				// carriage return
				string_append_uchar(&value, '\r');
				break;
			case 't':
				// tab
				string_append_uchar(&value, '\t');
				break;
			case 'u':
				// unicode escape sequence
				r = lexer_lex_unicode_literal(lexer, &chr);
				if (!r.success) {
					string_free(&value);
					return r;
				}
				r = utf8_append_encoded_codepoint(chr, &value);
				if (!r.success) {
					string_free(&value);
					return r;
				}
				break;
			default:
				string_free(&value);
				return new_errorf("Invalid escape character \\%c at line %zu, column %zu",
								  ELexerSyntaxError, chr, lexer->line, lexer->column - 2);
			}
			is_escaped = false;
		} else {
			switch (chr) {
			case '\\':
				is_escaped = true;
				break;
			case '"':
				token->type = JSONTok_String;
				token->value = value;
				return new_success();
			default:
				r = utf8_append_encoded_codepoint(chr, &value);
				if (!r.success) {
					string_free(&value);
					return r;
				}
				break;
			}
		}
	}
}


void lexer_free_token(JSONToken *token) { string_free(&token->value); }

const char *tk_as_str(JSONTokenType type) {
	if (type < 0 || type >= sizeof(JSONTokenTypeStrings) / sizeof(char *)) {
		panicf("Invalid JSONTokenType: %u", type);
	}
	return (const char *)JSONTokenTypeStrings[type];
}
