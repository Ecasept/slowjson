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
static Result lexer_lex_whitespace(Lexer *lexer, JSONToken *token, uchar start);
static Result lexer_lex_string(Lexer *lexer, JSONToken *token);
static Result lexer_lex_literal(Lexer *lexer, JSONToken *token);

const char *JSONTokenTypeStrings[] = {FOREACH_TOKEN(DECLARE_TOKEN_STRING)};

void lexer_init(Lexer *lexer, const string *source, ParserConfig config) {
	lexer->source = source;
	lexer->line = 1;
	lexer->column = 1;
	lexer->config = config;
	lexer->decoder = utf8_decoder_new(as_sv(*source));
}

static inline void lexer_lex_structural(JSONToken *token, uchar start) {
	switch (start) {
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
	}
}

Result lexer_next_token(Lexer *lexer, JSONToken *token) {
	token->column = lexer->column;
	token->line = lexer->line;
	token->type = JSONTok_Unknown;


	uchar current_char = 0;
	Result r = lexer_peek_uchar(lexer, &current_char);
	if (!r.success && cerrno.type == ELexerEOF) {
		token->type = JSONTok_EOF;
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
		lexer_skip(lexer, 1); // Consume structural character
		lexer_lex_structural(token, current_char);
		return new_success();
	case '\n':
	case '\r':
	case '\t':
	case ' ':
		lexer_skip(lexer, 1); // Consume whitespace character
		return lexer_lex_whitespace(lexer, token, current_char);
	case '"':
		lexer_skip(lexer, 1); // Consume opening quote
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
		return lexer_lex_number(lexer, token, current_char);
	default:;
		// Get codepoint
		UCP codepoint;
		check(lexer_peek(lexer, &codepoint));
		if (isalpha(codepoint)) {
			return lexer_lex_literal(lexer, token);
		} else {
			return new_errorf("Unexpected character \"%c\" at line %zu, column %zu (expected start of new token)",
							  ELexerSyntaxError, codepoint, lexer->line,
							  lexer->column);
		}
	}
}



static Result lexer_lex_whitespace(Lexer *lexer, JSONToken *token, uchar start) {
	uchar current_char;

	string whitespace;
	string_new(&whitespace, "");
	string_append_uchar(&whitespace, start);

	while (1) {
		Result r = lexer_peek_uchar(lexer, &current_char);
		if (!r.success && cerrno.type == ELexerEOF) {
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
			lexer_skip(lexer, 1); // Consume whitespace character
			if (current_char == '\n') {
				lexer->column = 1;
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

static bool lexer_test_literal(Lexer *lexer, string_view expect) {
	size_t current_pos = utf8_decoder_pos(&lexer->decoder);
	if (current_pos + expect.size > lexer->source->arr.length) {
		return false;
	}
	string_view substr = sv_substr_unchecked(
		as_sv(*lexer->source), current_pos, expect.size);

	return sv_eq(substr, expect);
}

static bool lexer_lex_specific_literal(Lexer *lexer, JSONToken *token,
								  string_view literal,
								  JSONTokenType type) {
	if (lexer_test_literal(lexer, literal)) {
		token->type = type;
		lexer_skip(lexer, literal.size);
		return true;
	}
	return false;
}

static Result lexer_lex_literal(Lexer *lexer, JSONToken *token) {
	if (lexer_lex_specific_literal(lexer, token, svl("true"), JSONTok_True)) {
		return new_success();
	}

	if (lexer_lex_specific_literal(lexer, token, svl("false"), JSONTok_False)) {
		return new_success();
	}
	if (lexer_lex_specific_literal(lexer, token, svl("null"), JSONTok_Null)) {
		return new_success();
	}

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
		if (!r1.success) {
			if (cerrno.type == ELexerEOF) {
				return new_error(
					"Unexpected end of file after high surrogate in unicode escape",
					ELexerSyntaxError);
			} else {
				return r1;
			}
		} else if (!r2.success) {
			if (cerrno.type == ELexerEOF) {
				return new_error(
					"Unexpected end of file after high surrogate in unicode escape",
					ELexerSyntaxError);
			} else {
				return r2;
			}
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
		if (!is_escaped) {
			// Fast path: read chunks of ASCII characters directly
			size_t start_idx = utf8_decoder_pos(&lexer->decoder);
			size_t current_idx = start_idx;

			const uchar *data = lexer->decoder.source.data;
			size_t max_idx = lexer->decoder.source.size;

			while (current_idx < max_idx) {
				uchar c = data[current_idx];
				// Stop at control characters, double quote, backslash or non-ASCII
				if (c < 0x20 || c == '"' || c == '\\' || c >= 0x80) {
					break;
				}
				current_idx++;
			}

			size_t len = current_idx - start_idx;
			if (len > 0) {
				string_append_bytes(&value, data + start_idx, len);
				lexer_skip(lexer, len);
			}
		}

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


void lexer_free_token(JSONToken *token) {
	if (token->type == JSONTok_String || token->type == JSONTok_Whitespace) {
		string_free(&token->value);
	}
}

const char *tk_as_str(JSONTokenType type) {
	if (type < 0 || type >= sizeof(JSONTokenTypeStrings) / sizeof(char *)) {
		panicf("Invalid JSONTokenType: %u", type);
	}
	return (const char *)JSONTokenTypeStrings[type];
}
