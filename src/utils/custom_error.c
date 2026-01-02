#include "custom_error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "dstring.h"
#include "unicode/wchar.h"

Result new_error(const char msg[], ErrorType type) {
	Result err;

	size_t len = strlen(msg);
	err.message = malloc(sizeof(char) * (len + 1));
	if (err.message == NULL) {
		panic("Failed to allocate memory");
	}
	strcpy(err.message, msg);

	err.success = false;
	err.type = type;
	return err;
}

Result new_success(void) {
	Result err;
	err.message = NULL;
	err.success = true;
	err.type = ESuccess;
	return err;
}

void print_error(Result r) {
    if (r.success) {
        wprintf(L"No error occurred.\n");
    } else {
        if (r.message != NULL) {
            wprintf(L"Error: %hs: %hs\n", etostr(r.type), r.message);
        } else {
            wprintf(L"Error: %hs\n", etostr(r.type));
        }
    }
}

string format_error(Result r) {
	if (r.success) {
		return string_newr("No error occurred.");
	} else {
		if (r.message != NULL) {
			string formatted = string_newr("Error: ");
			string_append_cstr(&formatted, etostr(r.type));
			string_append_cstr(&formatted, ": ");
			string_append_cstr(&formatted, r.message);
			return formatted;
		} else {
			string formatted = string_newr("Error: ");
			string_append_cstr(&formatted, etostr(r.type));
			return formatted;
		}
	}
}

wchar_t *format_error_wchar(Result r) {
	string formatted = format_error(r);
	wchar_t *wformatted = NULL;
	Result res = utf8_string_to_wchar(&formatted, &wformatted);
	string_free(&formatted);
	if (!res.success) {
		const wchar_t *fallback_msg = L"Error formatting error message.";
		size_t n = wcslen(fallback_msg) + 1;
		wchar_t *fallback = malloc(sizeof(wchar_t) * n);
		if (fallback == NULL) {
			panic("Failed to allocate memory");
		}
		wcscpy(fallback, fallback_msg);
		return fallback;
	}
	return wformatted;
}

Result new_errorf(const char *format, ErrorType type, ...) {
	va_list args;
	va_start(args, type);

	va_list args_copy;
	va_copy(args_copy, args);

	// Measure the length of the formatted string
	int len = vsnprintf(NULL, 0, format, args_copy);
	va_end(args_copy);

	if (len < 0) {
		va_end(args);
		panic("Failed to format error message");
	}

	char *str = malloc(len + 1);
	if (str == NULL) {
		va_end(args);
		panic("Failed to allocate memory");
	}

	vsnprintf(str, len + 1, format, args); // + 1 for null terminator
	va_end(args);

	Result err;
	err.message = str;
	err.success = false;
	err.type = type;
	return err;
}

void error_free(Result r) {
	if (r.message != NULL) {
		free(r.message);
	}
}

_Noreturn void panic(const char *message) {
	fprintf(stderr, "PANIC: %s\n", message);
	exit(EXIT_FAILURE);
}

_Noreturn void panicf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	fprintf(stderr, "PANIC: ");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(EXIT_FAILURE);
}

static const char *error_type_strings[] = {
	FOREACH_ERROR_TYPE(DECLARE_ERROR_TYPE_STRING)
};

const char* etostr(ErrorType type) {
	if (type < 0 || type >= sizeof(error_type_strings) / sizeof(error_type_strings[0])) {
		return "Unknown error";
	}
	return error_type_strings[type];
}
