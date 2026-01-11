#include "../deserialize.h"

Result json_get_typed(const JSONValue *obj, string_view key, JSONType type, JSONValue *out) {
    if (obj->type != JSON_OBJECT) {
        return new_error("Expected JSON object", ESaveFormatError);
    }
    
	check(json_value_hashmap_get(&obj->hashmap, key, out));
	
	if (out->type != type) {
		return new_errorf("Field '%.*s': expected %s, got %s",
						  ESaveFormatError, (int)key.size, key.data, jtostr(type), jtostr(out->type));
	}
    return new_success();
}

Result extract_int(const JSONValue *obj, string_view key, int *out) {
    JSONValue val;
    check(json_get_typed(obj, key, JSON_NUMBER, &val));
    if (!val.number.is_integer) {
        return new_errorf("Field '%.*s' must be an integer", ESaveFormatError, (int)key.size, key.data);
    }
    *out = (int)val.number.int_value;
    return new_success();
}
