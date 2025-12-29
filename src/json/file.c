#include "file.h"
#include <stdio.h>

// How much to read per iteration
const size_t READ_SIZE = 4096;

/**
 * @brief Allocates `str` and reads data from `filename` to it
 */
Result read_file_to_string(const string *filename, string *str) {
	char *filename_cstr;
	string_to_cstr(filename, &filename_cstr);

	FILE *fptr = fopen(filename_cstr, "rb");
	if (fptr == NULL) {
		Result r = new_errorf("Could not open file \"%s\"", EFailedToReadFile,
							  filename_cstr);
		free(filename_cstr);
		return r;
	}

	string_new(str, "");

	while (1) {
		uchar_list_ensure_resize(&str->arr, str->arr.length + READ_SIZE);
		unsigned char *ptr = str->arr.data + str->arr.length;
		size_t count = fread(ptr, sizeof(unsigned char), READ_SIZE, fptr);
		str->arr.length += count;
		if (count != READ_SIZE) {
			if (feof(fptr)) {
				break;
			} else if (ferror(fptr)) {
				string_free(str);
				fclose(fptr);
				Result r = new_errorf("Could not read file \"%s\"",
									  EFailedToReadFile, filename_cstr);
				free(filename_cstr);
				return r;
			} else {
				panic("Invalid error condition when reading file");
			}
		}
	}

	fclose(fptr);
	free(filename_cstr);
	return new_success();
}
