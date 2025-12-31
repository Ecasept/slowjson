#include "../utils/dstring.h"
#include "internal/jsonvalue.h"

void serialize_json(struct JSONValue *val, string *str);
Result deserialize_json(string *json, struct JSONValue *result);
