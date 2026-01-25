#pragma once
#include <stdbool.h>
#include "wchar.h"
#include <threads.h>
#define check(code) do {Result _r = (code); if (!_r.success) return _r;} while(0)

struct string;
typedef struct string string;

#define FOREACH_ERROR_TYPE(macro)                                                   \
	macro(ESuccess)                                     \
		macro(EFileOperationFailed) \
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
		macro(ESaveFormatError) 				 \
		macro(EFileNotFound) 				 \
		macro(ELexerNumberOverflow) \
		macro(ESerializeError) \
		macro(EDepthLimitExceeded)

#define DEFINE_ERROR_TYPE_ENUM(token) token,
#define DECLARE_ERROR_TYPE_STRING(token) #token,

typedef enum {
	FOREACH_ERROR_TYPE(DEFINE_ERROR_TYPE_ENUM)
} ErrorType;

const char* etostr(ErrorType type);

struct cerrno_t {
	char *message;
	ErrorType type;
};
typedef struct cerrno_t cerrno_t;

struct Result {
	bool success;
};
typedef struct Result Result;

struct RealResult {
	bool success;
	cerrno_t error;
};
typedef struct RealResult RealResult;

extern thread_local cerrno_t cerrno;

RealResult cerrno_store(Result r);
void real_result_free(RealResult rr);
string format_real_result(RealResult rr);
void cerrno_free(cerrno_t err);
string format_cerrno(Result r, cerrno_t err);

void set_cerrno(const char msg[], ErrorType type);
static inline Result new_error(const char msg[], ErrorType type) {
	set_cerrno(msg, type);
	return (Result){.success = false};
}
static inline Result new_success(void) {
	return (Result){ .success = true };
}
void print_error(Result r);
void error_prependf(Result *r, const char *format, ...)
	__attribute__((format(printf, 2, 3)));
string format_error(Result r);
wchar_t *format_error_wchar(Result r);
Result new_errorf(const char *format, ErrorType type, ...)
	__attribute__((format(printf, 1, 3)));
void error_free(Result r);

_Noreturn void panic(const char *message);
_Noreturn void panicf(const char *format, ...)
	__attribute__((format(printf, 1, 2)));
