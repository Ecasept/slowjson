#pragma once
#include <stdbool.h>
#include "wchar.h"
#define check(code) do {Result _r = (code); if (!_r.success) return _r;} while(0)

struct string;
typedef struct string string;

#define FOREACH_ERROR_TYPE(macro)                                                   \
	macro(ESuccess)                                     \
		macro(EFailedToReadFile) \
		macro(ELexerEOF) \
		macro(EParserUnexpectedEOF)      \
		macro(EParserSyntaxError) \
		macro(ELexerSyntaxError)                    \
		macro(ESubstrOutOfRange) \
		macro(ENotImplemented)                   \
		macro(EIndexOutOfBounds) \
		macro(EUnicodeError)                 \
		macro(EUnicodeUnexpectedEndOfString)                      \
		macro(EHashmapKeyNotFound) \
		macro(ESaveFormatError)

#define DEFINE_ERROR_TYPE_ENUM(token) token,
#define DECLARE_ERROR_TYPE_STRING(token) #token,

typedef enum {
	FOREACH_ERROR_TYPE(DEFINE_ERROR_TYPE_ENUM)
} ErrorType;

const char* etostr(ErrorType type);

typedef struct {
	char *message;
	bool success;
	ErrorType type;
} Result;

Result new_error(const char message[], ErrorType type);
Result new_success(void);
void print_error(Result r);
string format_error(Result r);
wchar_t *format_error_wchar(Result r);
Result new_errorf(const char *format, ErrorType type, ...)
	__attribute__((format(printf, 1, 3)));
void error_free(Result r);

_Noreturn void panic(const char *message);
_Noreturn void panicf(const char *format, ...)
	__attribute__((format(printf, 1, 2)));
