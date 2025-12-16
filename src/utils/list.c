#define TYPE char
#define TYPED_NAME(x) char_##x
#define LIST_IMPLEMENTATION
#include "list.h"

#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
