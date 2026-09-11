#include "utils/string/dstring.h"
#include "internal/jsonvalue.h"

/**
 * @brief Serializes a JSONValue into a string using a specific allocator.
 * @param val The JSON value to serialize
 * @param str Pointer to the string to store the result. Will be allocated.
 * @param allocator The allocator to use for the string
 */
Result json_serialize(const struct JSONValue *val, string *str, Allocator allocator);

