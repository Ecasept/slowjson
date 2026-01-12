#include "utils/string/dstring.h"
#include "internal/jsonvalue.h"
#include "config.h"

/**
 * @brief Deserializes a JSONValue from a string.
 * @param json Pointer to the string containing the JSON data
 * @param result Pointer to store the resulting JSONValue
 * @param config Parser configuration
 */
Result json_deserialize(string *json, struct JSONValue *result, ParserConfig config);

/**
 * @brief Gets a field from a json object and verifies its type.
 * 
 * @param obj The JSON object
 * @param key The key to look for
 * @param type The expected type
 * @param out Pointer to store the resulting JSONValue pointer
 */
Result json_get_typed(const JSONValue *obj, string_view key, JSONType type, JSONValue *out);

/**
 * @brief Extracts a number field from a JSON object and verifies it is an integer.
 * @param obj The JSON object
 * @param key The key to look for
 * @param out Pointer to store the resulting integer
 */
Result extract_int(const JSONValue *obj, string_view key, int *out);
