#pragma once
#include "utils.h"
#include <wchar.h>

Result utf8_get_next_codepoint(const string *str, size_t *index,
							   UnicodeCodePoint *out_codepoint);

Result utf8_append_encoded_codepoint(UnicodeCodePoint cp, string *str);
