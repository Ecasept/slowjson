#include "unicode/unicode_types.h"

#define TYPE uchar
#define TYPED_NAME(x) uchar_##x
#define LIST_IMPLEMENTATION
#define LIST_DECLARATION
#include "list.h"
#undef LIST_DECLARATION

#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
