#pragma once
#include "../utils/custom_error.h"
#include "../utils/string/dstring.h"
#include "../utils/unicode/unicode_types.h"
#include "../utils/unicode/utf8.h"
#include "jsonvalue.h"
#include "../config.h"
#include "../utils/alloc/allocator.h"

// Apply `macro` to each JSONTokenType
#define FOREACH_TOKEN(macro)                                                   \
	macro(JSONTok_EOF) macro(JSONTok_LBrace) macro(JSONTok_RBrace)             \
		macro(JSONTok_LBracket) macro(JSONTok_RBracket) macro(JSONTok_Colon)   \
			macro(JSONTok_Comma) macro(JSONTok_String) macro(JSONTok_Number)   \
				macro(JSONTok_True) macro(JSONTok_False) macro(JSONTok_Null)   \
					macro(JSONTok_Whitespace) macro(JSONTok_Unknown)


#undef DECLARE_TOKEN_STRING
#define FRIENDLY_NAME_JSONTok_EOF "end of input"
#define FRIENDLY_NAME_JSONTok_LBrace "'{'"
#define FRIENDLY_NAME_JSONTok_RBrace "'}'"
#define FRIENDLY_NAME_JSONTok_LBracket "'['"
#define FRIENDLY_NAME_JSONTok_RBracket "']'"
#define FRIENDLY_NAME_JSONTok_Colon "':'"
#define FRIENDLY_NAME_JSONTok_Comma "','"
#define FRIENDLY_NAME_JSONTok_String "string"
#define FRIENDLY_NAME_JSONTok_Number "number"
#define FRIENDLY_NAME_JSONTok_True "'true'"
#define FRIENDLY_NAME_JSONTok_False "'false'"
#define FRIENDLY_NAME_JSONTok_Null "'null'"
#define FRIENDLY_NAME_JSONTok_Whitespace "whitespace"
#define FRIENDLY_NAME_JSONTok_Unknown "unknown token"

#define DECLARE_TOKEN_STRING(token) FRIENDLY_NAME_##token,
#define DEFINE_TOKEN_ENUM(token) token,

typedef enum { FOREACH_TOKEN(DEFINE_TOKEN_ENUM) } JSONTokenType;
extern const char *JSONTokenTypeStrings[];

const char *tk_as_str(JSONTokenType type);

typedef struct {
	JSONTokenType type;
	size_t line;
	size_t column;
	union {
		string value;
		JSONNumber number;
	};
} JSONToken;

typedef struct {
	const string *source;
	size_t line;
	size_t column;
	ParserConfig config;
	UTF8Decoder decoder;
	Allocator allocator;
} Lexer;

void lexer_init(Lexer *lexer, const string *source, ParserConfig config, Allocator a);
Result lexer_next_token(Lexer *lexer, JSONToken *token);
void lexer_free_token(Lexer *lexer, JSONToken *token);
