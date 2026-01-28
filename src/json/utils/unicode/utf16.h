#include "utils.h"

bool is_surrogate_half(UnicodeCodePoint cp);
bool is_low_surrogate(UnicodeCodePoint cp);
bool is_high_surrogate(UnicodeCodePoint cp);
UnicodeCodePoint decode_surrogate_pair(UnicodeCodePoint high,
									   UnicodeCodePoint low);
bool needs_surrogate_pair(UnicodeCodePoint cp);
void encode_surrogate_pair(UnicodeCodePoint cp, UnicodeCodePoint *high,
							UnicodeCodePoint *low);
