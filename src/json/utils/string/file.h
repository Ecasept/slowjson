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





struct FileLineIterator {
	FILE *fptr;
	string current_line;
	bool eof_reached;
};
typedef struct FileLineIterator FileLineIterator;

/**
 * @brief Allocates `str` and reads data from `filename` to it
 */
Result read_lines(const char *filename, FileLineIterator *out_iterator);

Result file_line_iterator_close(FileLineIterator *iterator);

Result file_line_iterator_next(FileLineIterator *iterator, bool *has_line);
