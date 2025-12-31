#include "../dstring.h"

Result wchar_to_utf8_string(const wchar_t *src, string *dest);
Result utf8_string_to_wchar(const string *src, wchar_t **dest);


#define TYPE wchar_t
#define TYPED_NAME(name) wchar_##name
#define LIST_DECLARATION
#include "../../utils/list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME
