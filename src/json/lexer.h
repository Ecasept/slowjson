#include "../utils/custom_error.h"
#include "../utils/dstring.h"

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

static const char *JSONTokenTypeStrings[] = {
	FOREACH_TOKEN(DECLARE_TOKEN_STRING)};

char *tk_as_str(JSONTokenType type);

typedef struct {
	JSONTokenType type;
	string value;
	size_t line;
	size_t column;
} JSONToken;

typedef struct {
	const string *source;
	size_t position;
	size_t line;
	size_t column;
} Lexer;

void lexer_init(Lexer *lexer, const string *source);
Result lexer_next_token(Lexer *lexer, JSONToken *token);
void lexer_free_token(JSONToken *token);

Result lexer_peek(const Lexer *lexer, char *out);
Result lexer_consume(Lexer *lexer, char *out);

Result lexer_lex_structural(Lexer *lexer, JSONToken *token);
Result lexer_lex_whitespace(Lexer *lexer, JSONToken *token);
Result lexer_lex_string(Lexer *lexer, JSONToken *token);
Result lexer_lex_number(Lexer *lexer, JSONToken *token);
Result lexer_lex_literal(Lexer *lexer, JSONToken *token);
Result lexer_test_literal(Lexer *lexer, string *string, bool *res);
