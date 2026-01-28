#include "../string/dstring.h"
#include "../alloc/allocator.h"

Result wchar_to_utf8_string(const wchar_t *src, string *dest, Allocator a);
Result utf8_string_to_wchar(string_view src, wchar_t **dest, Allocator a);


#define TYPE wchar_t
#define TYPED_NAME(name) wchar_##name
#define LIST_DECLARATION
#include "../list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME
