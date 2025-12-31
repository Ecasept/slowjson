#pragma once
#include "../utils/custom_error.h"
#include "../utils/dstring.h"

extern const char *JSON_SAVEFILE_NAME;

Result read_file_to_string(const char *filename, string *str);
Result write_string_to_file(const char *filename, const string *str);
