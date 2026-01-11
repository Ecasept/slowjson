#include "utils/string/dstring.h"
#include "internal/jsonvalue.h"

/**
 * @brief Serializes a JSONValue into a string.
 * @param val The JSON value to serialize
 * @param str Pointer to the string to write to. The string must be initialized.
 * The serialized data will be appended to it.
 */
void json_serialize(struct JSONValue *val, string *str);

