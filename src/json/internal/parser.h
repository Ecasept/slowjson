#pragma once
#include "jsonvalue.h"
#include "lexer.h"
#include "../config.h"
#include "../document.h"


struct Parser {
	Lexer *lexer;
	JSONToken lookahead;
	bool has_lookahead;
	ParserConfig config;
	JSONDocument *document;
};
typedef struct Parser Parser;
