#include "file.h"
#include <stdio.h>

// How much to read per iteration
const size_t READ_SIZE = 4096;
const char *JSON_SAVEFILE_NAME = "data.json";


/**
 * @brief Allocates `str` and reads data from `filename` to it
 */
Result read_file_to_string(const char *filename, string *str) {
	FILE *fptr = fopen(filename, "rb");
	if (fptr == NULL) {
		return new_errorf("Could not open file \"%s\"", EFailedToReadFile,
						  filename);
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
				return new_errorf("Could not read file \"%s\"",
									  EFailedToReadFile, filename);
			} else {
				panic("Invalid error condition when reading file");
			}
		}
	}

	fclose(fptr);
	return new_success();
}

Result write_string_to_file(const char *filename, const string *str) {
	FILE *fptr = fopen(filename, "wb");
	if (fptr == NULL) {
		return new_errorf("Could not open file \"%s\" for writing",
						  EFailedToReadFile, filename);
	}

	size_t written = fwrite(str->arr.data, sizeof(uchar), str->arr.length, fptr);
	if (written != str->arr.length) {
		fclose(fptr);
		return new_errorf("Could not write to file \"%s\"", EFailedToReadFile,
						  filename);
	}

	fclose(fptr);
	return new_success();
}
