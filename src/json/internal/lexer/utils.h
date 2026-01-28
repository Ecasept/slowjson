#pragma once
#include "../lexer.h"

static inline Result lexer_peek(Lexer *lexer, UCP *out) {
	Result r = utf8_decoder_peek(&lexer->decoder, out);
	if (!r.success) {
		if (cerrno.type == EUnicodeUnexpectedEndOfString) {
			error_free(r);
			return new_error("Unexpected end of input", ELexerEOF);
		}
		return r;
	}
	return new_success();
}

static inline void lexer_skip(Lexer *lexer, size_t amount) {
	utf8_decoder_skip(&lexer->decoder, amount);
	lexer->column += amount;
}

static inline Result lexer_consume(Lexer *lexer, UCP *out) {
	Result r = utf8_decoder_next(&lexer->decoder, out);
	if (!r.success) {
		if (cerrno.type == EUnicodeUnexpectedEndOfString) {
			error_free(r);
			return new_error("Unexpected end of input", ELexerEOF);
		}
		return r;
	}
	lexer->column++;
	return new_success();
}

static inline Result lexer_peek_uchar(Lexer *lexer, uchar *out) {
	Result r = utf8_decoder_peek_uchar(&lexer->decoder, out);
	if (!r.success) {
		if (cerrno.type == EUnicodeUnexpectedEndOfString) {
			error_free(r);
			return new_error("Unexpected end of input", ELexerEOF);
		}
		return r;
	}
	return new_success();
}
