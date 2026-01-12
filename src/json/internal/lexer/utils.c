#include "utils.h"
#include "../../utils/unicode/utf8.h"

Result lexer_peek(const Lexer *lexer, UCP *out) {
	size_t pos_copy =
		lexer->position; // Don't pass the actual position. We are just peeking
	Result r = utf8_get_next_codepoint(as_sv(*lexer->source), &pos_copy, out);
	if (r.type == EUnicodeUnexpectedEndOfString) {
		error_free(r);
		return new_error("Unexpected end of input", ELexerEOF);
	} else if (!r.success) {
		return r;
	}
	return new_success();
}

Result lexer_consume(Lexer *lexer, UCP *out) {
	Result r = utf8_get_next_codepoint(as_sv(*lexer->source), &lexer->position, out);
	if (r.type == EUnicodeUnexpectedEndOfString) {
		error_free(r);
		return new_error("Unexpected end of input", ELexerEOF);
	} else if (!r.success) {
		return r;
	}
	lexer->column++;
	return new_success();
}
