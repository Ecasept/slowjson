#include "file.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

// How much to read per iteration
static const size_t READ_SIZE = 4096;
const char *JSON_SAVEFILE_NAME = "data.json";


/**
 * @brief Allocates `str` and reads data from `filename` to it
 */
Result read_file_to_string(const char *filename, string *str) {
	FILE *fptr = fopen(filename, "rb");
	if (fptr == NULL) {
        ErrorType type = (errno == ENOENT) ? EFileNotFound : EFileOperationFailed;
        return new_errorf("Could not open file \"%s\": %s", type,
                          filename, strerror(errno));

	}

	string_new(str, "");

	while (1) {
		uchar_list_ensure_resize(&str->arr, str->arr.length + READ_SIZE);
		unsigned char *ptr = str->arr.data + str->arr.length;
		size_t count = fread(ptr, sizeof(unsigned char), READ_SIZE, fptr);
		str->arr.length += count;
		if (count != READ_SIZE) {
           if (ferror(fptr)) {
                int err = errno;
                string_free(str);
                fclose(fptr);
                return new_errorf("Could not read file \"%s\": %s",
                                  EFileOperationFailed, filename,
                                  (err != 0) ? strerror(err) : "unknown I/O error");
            }
            if (feof(fptr)) {
                break;
            }
            panic("Unreachable code reached in read_file_to_string");
		}
	}

    if (fclose(fptr) == EOF) {
        int err = errno;
        string_free(str);
        return new_errorf("Could not close file \"%s\": %s",
                          EFileOperationFailed, filename,
                          (err != 0) ? strerror(err) : "unknown close error");
    }

	return new_success();
}

Result write_string_to_file(const char *filename, const string *str) {
	FILE *fptr = fopen(filename, "wb");
    if (fptr == NULL) {
		ErrorType type = (errno == ENOENT) ? EFileNotFound : EFileOperationFailed;
        return new_errorf("Could not open file \"%s\" for writing: %s",
                          type, filename, strerror(errno));
    }

	size_t written = fwrite(str->arr.data, sizeof(uchar), str->arr.length, fptr);
    if (written != str->arr.length) {
        int err = errno;
        fclose(fptr);
        return new_errorf("Could not write to file \"%s\": %s",
                          EFileOperationFailed, filename,
                          (err != 0) ? strerror(err) : "unknown write error");
    }

    if (fclose(fptr) == EOF) {
        int err = errno;
        return new_errorf("Could not close file \"%s\": %s",
                          EFileOperationFailed, filename,
                          (err != 0) ? strerror(err) : "unknown close error");
    }
	return new_success();
}
