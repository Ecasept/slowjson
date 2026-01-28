#include "custom_error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "string/dstring.h"
#include "string/string_view.h"
#include "unicode/wchar.h"
#include "alloc/default.h"

cerrno_t cerrno;

void set_cerrno(const char msg[], ErrorType type) {
	size_t len = strlen(msg);
	cerrno.message = malloc(sizeof(char) * (len + 1));
	if (cerrno.message == NULL) {
		panic("Failed to allocate memory");
	}
	strcpy(cerrno.message, msg);

	cerrno.type = type;
}

void print_error(Result r) {
    if (r.success) {
        wprintf(L"No error occurred.\n");
    } else {
        if (cerrno.message != NULL) {
            wprintf(L"Error: %hs: %hs\n", etostr(cerrno.type), cerrno.message);
        } else {
            wprintf(L"Error: %hs\n", etostr(cerrno.type));
        }
    }
}

void error_prependf(Result *r, const char *format, ...) {
	if (r->success) {
		return;
	}

	va_list args;
	va_start(args, format);

	va_list args_copy;
	va_copy(args_copy, args);

	// Measure the length of the formatted string
	int len = vsnprintf(NULL, 0, format, args_copy);
	va_end(args_copy);

	if (len < 0) {
		va_end(args);
		panic("Failed to format error message");
	}

	char *prefix = malloc(len + 1);
	if (prefix == NULL) {
		va_end(args);
		panic("Failed to allocate memory");
	}

	vsnprintf(prefix, len + 1, format, args); // + 1 for null terminator
	va_end(args);

	// Create new message
	size_t old_len = cerrno.message ? strlen(cerrno.message) : 0;
	char *new_message = malloc(len + old_len + 1);
	if (new_message == NULL) {
		free(prefix);
		panic("Failed to allocate memory");
	}

	strcpy(new_message, prefix);
	if (cerrno.message) {
		strcat(new_message, cerrno.message);
		free(cerrno.message);
	}

	free(prefix);
	cerrno.message = new_message;
}

string format_cerrno(Result r, cerrno_t err) {
	if (r.success) {
		return string_newr("No error occurred.", ga);
	} else {
		if (err.message != NULL) {
			string formatted = string_newr("Error: ", ga);
			string_append_cstr(&formatted, etostr(err.type), ga);
			string_append_cstr(&formatted, ": ", ga);
			string_append_cstr(&formatted, err.message, ga);
			return formatted;
		} else {
			string formatted = string_newr("Error: ", ga);
			string_append_cstr(&formatted, etostr(err.type), ga);
			return formatted;
		}
	}
}

string format_real_result(RealResult rr) {
	return format_cerrno((Result){.success = rr.success}, rr.error);
}

string format_error(Result r) {
	return format_cerrno(r, cerrno);
}

wchar_t *format_error_wchar(Result r) {
	string formatted = format_error(r);
	wchar_t *wformatted = NULL;
	Result res = utf8_string_to_wchar(as_sv(formatted), &wformatted, ga);
	string_free(&formatted, ga);
	if (!res.success) {
		// Do not try to return `res.message` as that may also fail to convert to wchar_t
		error_free(res);
		const wchar_t *fallback_msg = L"Error converting error message to wide characters";
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

	cerrno.message = str;
	cerrno.type = type;
	return (Result){.success = false};
}

void cerrno_free(cerrno_t err) {
	if (err.message != NULL) {
		free(err.message);
	}
}

void error_free(Result r) {
	if (!r.success) {
		cerrno_free(cerrno);
		cerrno.message = NULL;
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

RealResult cerrno_store(Result r) {
	RealResult rr = { .success = r.success };
	if (!r.success) {
		rr.error = cerrno;
		cerrno.message = NULL;
		cerrno.type = ESuccess;
	} else {
		rr.error = (cerrno_t){0};
	}
	return rr;
}

void real_result_free(RealResult rr) {
	if (!rr.success) {
		cerrno_free(rr.error);
	}
}
