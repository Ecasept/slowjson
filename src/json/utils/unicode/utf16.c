#include "utf16.h"

bool is_surrogate_half(UnicodeCodePoint cp) {
	return cp >= 0xD800 && cp <= 0xDFFF;
}
bool is_low_surrogate(UnicodeCodePoint cp) {
	return cp >= 0xDC00 && cp <= 0xDFFF;
}
bool is_high_surrogate(UnicodeCodePoint cp) {
	return cp >= 0xD800 && cp <= 0xDBFF;
}

UnicodeCodePoint decode_surrogate_pair(UnicodeCodePoint high,
									   UnicodeCodePoint low) {
	return (((high & b(1111111111))<< 10) | ((low & b(1111111111)))) + (1<<16);
}

bool needs_surrogate_pair(UnicodeCodePoint cp) {
	return cp >= 0x10000 && cp <= 0x10FFFF;
}
void encode_surrogate_pair(UnicodeCodePoint cp, UnicodeCodePoint *high,
							UnicodeCodePoint *low) {
	*high = (((cp - (1<<16)) >> 10) & b(1111111111)) + b(1101100000000000);
	*low = (cp & b(1111111111)) + b(1101110000000000);
}
