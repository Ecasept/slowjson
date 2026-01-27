#include "file.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../alloc/default.h"

// How much to read per iteration
static const size_t READ_SIZE = 4096;


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

	string_new(str, "", ga);

	while (1) {
		uchar_list_ensure_resize(&str->arr, str->arr.length + READ_SIZE, ga);
		unsigned char *ptr = str->arr.data + str->arr.length;
		size_t count = fread(ptr, sizeof(unsigned char), READ_SIZE, fptr);
		str->arr.length += count;
		if (count != READ_SIZE) {
           if (ferror(fptr)) {
                int err = errno;
				string_free(str, ga);
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
		string_free(str, ga);
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


Result read_lines(const char *filename, FileLineIterator *out_iterator) {
	FILE *fptr = fopen(filename, "rb");
	if (fptr == NULL) {
        ErrorType type = (errno == ENOENT) ? EFileNotFound : EFileOperationFailed;
        return new_errorf("Could not open file \"%s\": %s", type,
                          filename, strerror(errno));
	}

	out_iterator->fptr = fptr;
	out_iterator->eof_reached = false;
	string_new(&out_iterator->current_line, "", ga);

	return new_success();
}



Result file_line_iterator_close(FileLineIterator *iterator) {
	if (iterator->fptr != NULL) {
		if (fclose(iterator->fptr) == EOF) {
			int err = errno;
			string_free(&iterator->current_line, ga);
			return new_errorf("Could not close file: %s",
							  EFileOperationFailed,
							  (err != 0) ? strerror(err) : "unknown close error");
		}
		iterator->fptr = NULL;
	}
	string_free(&iterator->current_line, ga);
	return new_success();
}


Result file_line_iterator_next(FileLineIterator *iterator, bool *has_line) {
	if (iterator->eof_reached) {
		*has_line = false;
		return new_success();
	}

	// Clear current line (keep capacity to avoid reallocations)
	iterator->current_line.arr.length = 0;

	while (1) {
		int ch = fgetc(iterator->fptr);
		if (ch == EOF) {
			if (ferror(iterator->fptr)) {
				int err = errno;
				return new_errorf("Could not read from file: %s",
								  EFileOperationFailed,
								  (err != 0) ? strerror(err) : "unknown I/O error");
			}
			// EOF reached
			iterator->eof_reached = true;
			if (iterator->current_line.arr.length == 0) {
				*has_line = false;
				return new_success();
			} else {
				break;
			}
		}
		if (ch == '\n') {
			// End of line
			break;
		} else if (ch == '\r') {
			// Ignore carriage return
			continue;
		}
		string_append_uchar(&iterator->current_line, (uchar)ch, ga);
	}

	*has_line = true;
	return new_success();
}
