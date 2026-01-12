#include "utils/string/dstring.h"
#include "internal/jsonvalue.h"

/**
 * @brief Serializes a JSONValue into a string.
 * @param val The JSON value to serialize
 * @param str Pointer to the string to store the result. Will be allocated.
 */
Result json_serialize(struct JSONValue *val, string *str);

