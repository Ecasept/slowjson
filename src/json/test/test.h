#pragma once
#include "../deserialize.h"
#include "../utils/string/file.h"



/**
 * @brief Returns the result of attempting to parse the JSON file at the given filename.
 */
Result run_once(const char *filename);
/**
 * @brief Parses the given JSON string and outputs the resulting JSONValue.
 * @param json The JSON string to parse
 * @param out_result If non-NULL, will be set to the resulting JSONDocument on success.
 * If NULL, the resulting JSONDocument will be freed immediately.
 */
Result run_string(string *json, JSONDocument *out_result);
