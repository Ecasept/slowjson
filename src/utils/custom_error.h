#pragma once
#include <stdbool.h>
#define try(code)                                                              \
	r = code;                                                                  \
	if (!r.success) {                                                          \
		return r;                                                              \
	}

typedef enum {
	ESuccess,
	// Unrecoverable Errors
	CEAllocFailed,
	CEAlreadyFreed,
	CEIndexOutOfBounds,
	// Recoverable Errors
	EFailedToReadFile,
	ELexerEOF,
	ELexerSyntaxError,
	ESubstrOutOfRange
} ErrorType;

typedef struct {
	char *message;
	bool success;
	ErrorType type;
} Result;

Result new_error(const char message[], ErrorType type);
Result new_success();
void print_error(Result r);
Result new_errorf(const char *format, ErrorType type, ...)
	__attribute__((format(printf, 1, 3)));
void error_free(Result r);

_Noreturn void panic(const char *message);
_Noreturn void panicf(const char *format, ...)
	__attribute__((format(printf, 1, 2)));
