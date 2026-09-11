#pragma once
#include "jsonvalue.h"
#include "lexer.h"
#include "../config.h"
#include "../document.h"


#define TYPE JSONToken
#define TYPED_NAME(name) json_token_##name
#define LIST_DECLARATION
#include "../utils/list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME

struct Parser {
	json_token_list *tokens;
	size_t position;
	ParserConfig config;
	JSONDocument *document;
};
typedef struct Parser Parser;
