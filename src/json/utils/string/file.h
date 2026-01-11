#pragma once
#include "../custom_error.h"
#include "dstring.h"

/**
 * @brief Allocates `str` and reads data from `filename` to it
 */
Result read_file_to_string(const char *filename, string *str);
/**
 * @brief Writes the contents of `str` to `filename`
 */
Result write_string_to_file(const char *filename, const string *str);
