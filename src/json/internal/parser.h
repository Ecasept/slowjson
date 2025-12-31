#pragma once
#include "jsonvalue.h"
#include "lexer.h"

#define TYPE JSONToken
#define TYPED_NAME(name) json_token_##name
#define LIST_DECLARATION
#include "../../utils/list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME
