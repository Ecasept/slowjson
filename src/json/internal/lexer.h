#pragma once
#include "../utils/custom_error.h"
#include "../utils/string/dstring.h"
#include "../utils/unicode/unicode_types.h"
#include "jsonvalue.h"
#include "../config.h"

// Apply `macro` to each JSONTokenType
#define FOREACH_TOKEN(macro)                                                   \
	macro(JSONTok_EOF) macro(JSONTok_LBrace) macro(JSONTok_RBrace)             \
		macro(JSONTok_LBracket) macro(JSONTok_RBracket) macro(JSONTok_Colon)   \
			macro(JSONTok_Comma) macro(JSONTok_String) macro(JSONTok_Number)   \
				macro(JSONTok_True) macro(JSONTok_False) macro(JSONTok_Null)   \
					macro(JSONTok_Whitespace) macro(JSONTok_Unknown)

#define DEFINE_TOKEN_ENUM(token) token,
#define DECLARE_TOKEN_STRING(token) #token,

typedef enum { FOREACH_TOKEN(DEFINE_TOKEN_ENUM) } JSONTokenType;
extern const char *JSONTokenTypeStrings[];

const char *tk_as_str(JSONTokenType type);

typedef struct {
	JSONTokenType type;
	size_t line;
	size_t column;
	string value;
	JSONNumber number;
} JSONToken;

typedef struct {
	const string *source;
	size_t position;
	size_t line;
	size_t column;
	ParserConfig config;
} Lexer;

void lexer_init(Lexer *lexer, const string *source, ParserConfig config);
Result lexer_next_token(Lexer *lexer, JSONToken *token);
void lexer_free_token(JSONToken *token);
