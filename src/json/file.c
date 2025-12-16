#include "file.h"
#include <stdio.h>

/**
 * @brief Allocates `str` and reads data from `filename` to it
 */
Result read_file_to_string(const string *filename, string *str) {
	char *filename_cstr;
	string_to_cstr(filename, &filename_cstr);

	FILE *fptr = fopen(filename_cstr, "r");
	if (fptr == NULL) {
		free(filename_cstr);
		return new_errorf("Could not read file \"%s\"", EFailedToReadFile,
						  filename_cstr);
	}

	string_new(str, "");

	char read[100];
	while (fgets(read, 100, fptr)) {
		string_append_cstr(str, read);
	}

	fclose(fptr);
	free(filename_cstr);
	return new_success();
}
