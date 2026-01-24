#pragma once
#include "utils.h"
#include <wchar.h>

Result utf8_append_encoded_codepoint(UnicodeCodePoint cp, string *str);

struct UTF8Decoder {
	string_view source;
	size_t index;
};
typedef struct UTF8Decoder UTF8Decoder;

UTF8Decoder utf8_decoder_new(string_view source);

Result utf8_get_next_codepoint(string_view str, size_t *index,
							   UnicodeCodePoint *out);

static inline Result utf8_decoder_peek(UTF8Decoder *decoder, UnicodeCodePoint *out_codepoint) {
	size_t index = decoder->index;
	return utf8_get_next_codepoint(decoder->source, &index, out_codepoint);
}

static inline Result utf8_decoder_peek_uchar(UTF8Decoder *decoder, uchar *out_uchar) {
	if (decoder->index >= decoder->source.size) {
		return new_error(
			"Unexpected end of unicode string while reading character",
			EUnicodeUnexpectedEndOfString);
	}
	*out_uchar = sv_at_unchecked(decoder->source, decoder->index);
	return new_success();
}

static inline Result utf8_decoder_next(UTF8Decoder *decoder, UnicodeCodePoint *out_codepoint) {
	return utf8_get_next_codepoint(decoder->source, &decoder->index, out_codepoint);
}

static inline void utf8_decoder_skip(UTF8Decoder *decoder, size_t count) {
	decoder->index += count;
}

/** 
 * @brief Returns the current position in the source string
 */
static inline size_t utf8_decoder_pos(UTF8Decoder *decoder) {
	return decoder->index;
}
