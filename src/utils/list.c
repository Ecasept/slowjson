#include "unicode/unicode_types.h"

#define TYPE uchar
#define TYPED_NAME(x) uchar_##x
#define LIST_IMPLEMENTATION
#include "list.h"

#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
