#pragma once
#define TYPE uchar
#define TYPED_NAME(name) uchar_##name
#define LIST_DECLARATION
#include "../list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME

struct string {
	uchar_list arr;
};
typedef struct string string;
