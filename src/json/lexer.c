
#include "lexer.h"
#include <ctype.h>

// JSON Specification:
// https://www.json.org/json-en.html
// https://datatracker.ietf.org/doc/html/rfc8259
// https://ecma-international.org/publications-and-standards/standards/ecma-404/

void lexer_init(Lexer *lexer, const string *source) {
	lexer->source = source;
	lexer->position = 0;
	lexer->line = 0;
	lexer->column = 0;
}

Result lexer_peek(const Lexer *lexer, char *out) {
	if (lexer->position >= lexer->source->arr.length) {
		return new_error("Reached end of file", ELexerEOF);
	}
	string_at(lexer->source, lexer->position, out);
	return new_success();
}

Result lexer_consume(Lexer *lexer, char *out) {
	if (lexer->position >= lexer->source->arr.length) {
		return new_error("Reached end of file", ELexerEOF);
	}
	string_at(lexer->source, lexer->position, out);
	lexer->position++;
	lexer->column++;
	return new_success();
}

Result lexer_next_token(Lexer *lexer, JSONToken *token) {
	token->column = lexer->column;
	token->line = lexer->line;
	token->type = JSONTok_Unknown;

	Result r;

	char current_char;
	r = lexer_peek(lexer, &current_char);
	if (r.type == ELexerEOF) {
		token->type = JSONTok_EOF;
		string_new(&token->value, "");
		error_free(r);
		return new_success();
	}
	try(r);

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
			return new_errorf("Unexpected character \"%c\"", ELexerSyntaxError,
							  current_char);
		}
	}
}

Result lexer_lex_structural(Lexer *lexer, JSONToken *token) {
	// Implementation for lexing structural characters
	char current_char;
	Result r;
	try(lexer_consume(lexer, &current_char));

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
		return new_errorf("Unexpected character \"%c\"", ELexerSyntaxError,
						  current_char);
	}

	string_new(&token->value, "");
	string_append_char(&token->value, current_char);
	return new_success();
}

Result lexer_lex_whitespace(Lexer *lexer, JSONToken *token) {
	char current_char;

	string whitespace;
	string_new(&whitespace, "");

	Result r;

	while (1) {
		r = lexer_peek(lexer, &current_char);
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
			try(lexer_consume(lexer, &current_char));
			if (current_char == '\n') {
				lexer->column = 0;
				lexer->line++;
			}

			// Append whitespace character to current whitespace
			string_append_char(&whitespace, current_char);
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

Result lexer_test_literal(Lexer *lexer, string *expect, bool *res) {
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

Result lexer_lex_specific_literal(Lexer *lexer, JSONToken *token,
								  string *literal, bool *matched,
								  JSONTokenType type) {
	bool matches;
	Result r;
	try(lexer_test_literal(lexer, literal, &matches));
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

Result lexer_lex_literal(Lexer *lexer, JSONToken *token) {
	Result r;
	string true_str;
	string_new(&true_str, "true");
	bool matches;
	try(lexer_lex_specific_literal(lexer, token, &true_str, &matches,
								   JSONTok_True));
	if (matches) {
		return new_success();
	}
	string_free(&true_str);

	string false_str;
	string_new(&false_str, "false");
	try(lexer_lex_specific_literal(lexer, token, &false_str, &matches,
								   JSONTok_False));
	if (matches) {
		return new_success();
	}
	string_free(&false_str);

	string null_str;
	string_new(&null_str, "null");
	try(lexer_lex_specific_literal(lexer, token, &null_str, &matches,
								   JSONTok_Null));
	if (matches) {
		return new_success();
	}
	string_free(&null_str);

	return new_errorf("Unexpected literal", ELexerSyntaxError);
}

Result lexer_lex_string(Lexer *lexer, JSONToken *token) {
	return new_errorf("String lexing not implemented", ELexerSyntaxError);
}

Result lexer_lex_number(Lexer *lexer, JSONToken *token) {
	return new_errorf("Number lexing not implemented", ELexerSyntaxError);
}

void lexer_free_token(JSONToken *token) { string_free(&token->value); }

char *tk_as_str(JSONTokenType type) {
	if (type < 0 || type >= sizeof(JSONTokenTypeStrings) / sizeof(char *)) {
		panicf("Invalid JSONTokenType: %d", type);
	}
	return (char *)JSONTokenTypeStrings[type];
}
