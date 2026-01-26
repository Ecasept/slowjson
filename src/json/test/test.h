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
 * @param out_root If non-NULL, will be set to the resulting JSONValue on success.
 * If NULL, the resulting JSONValue (and parser) will be freed immediately.
 * @param out_parser If non-NULL, will be set to the parser used for parsing.
 */
Result run_string(string *json, JSONValue *out_root, Parser *out_parser);
