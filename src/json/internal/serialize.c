#include "../utils/hashmap/hashmap_node.h"
#include "../serialize.h"
#include <math.h>
#include "../utils/alloc/default.h"


static Result json_serialize_rec(JSONValue *val, string *str, Allocator a);

static Result json_serialize_rec(JSONValue *val, string *str, Allocator a) {
	switch (val->type) {
	case JSON_NULL:
		string_append_cstr(str, "null", a);
		break;
	case JSON_BOOL:
		string_append_cstr(str, val->boolean ? "true" : "false", a);
		break;
	case JSON_NUMBER:
		{
			char buf[64];
			if (val->number.is_integer) {
				sprintf(buf, "%lld", (long long)val->number.int_value);
			} else {
				if (isnan(val->number.float_value)) {
					return new_error("Cannot serialize NaN value", ESerializeError);
				}
				if (isinf(val->number.float_value)) {
					return new_error("Cannot serialize infinite value", ESerializeError);
				}
				snprintf(buf, sizeof(buf), "%f", val->number.float_value);
			}
			string_append_cstr(str, buf, a);
		}
		break;
	case JSON_STRING:
		string_append_uchar(str, '"', a);
		// A byte <= 0x7F may only appear representing a single-byte UTF-8 character
		// So we can iterate over the bytes directly instead of decoding codepoints
		for (size_t i = 0; i < val->str.arr.length; i++) {
			uchar c = val->str.arr.data[i];
			switch (c) {
			case '"':
				string_append_cstr(str, "\\\"", a);
				break;
			case '\n':
				string_append_cstr(str, "\\n", a);
				break;
			case '\t':
				string_append_cstr(str, "\\t", a);
				break;
			case '\\':
				string_append_cstr(str, "\\\\", a);
				break;
			case '\r':
				string_append_cstr(str, "\\r", a);
				break;
			case '\b':
				string_append_cstr(str, "\\b", a);
				break;
			case '\f':
				string_append_cstr(str, "\\f", a);
				break;
			default:
				if (c <= 0x1F) {
					// Control character
					char buf[7];
					sprintf(buf, "\\u%04X", c);
					string_append_cstr(str, buf, a);
				} else {
					string_append_uchar(str, c, a);
				}
				break;
			}
		}
		string_append_uchar(str, '"', a);
		break;
	case JSON_ARRAY:
		string_append_uchar(str, '[', a);
		for (size_t i = 0; i < val->list.length; i++) {
			if (i > 0) {string_append_uchar(str, ',', a);}
			Result r = json_serialize_rec(&val->list.data[i], str, a);
			if (!r.success) return r;
		}
		string_append_uchar(str, ']', a);
		break;
	case JSON_OBJECT:
		string_append_uchar(str, '{', a);
		bool first = true;
		for (size_t i = 0; i < val->hashmap.buckets.length; i++) {
			json_value_hashmap_node *node = &val->hashmap.buckets.data[i];
			while (node && node->key.arr.data != NULL) {
				if (!first) string_append_uchar(str, ',', a);
				JSONValue key = json_value_new_string(&node->key);
				check(json_serialize_rec(&key, str, a));
				string_append_uchar(str, ':', a);
				Result r = json_serialize_rec(&node->value, str, a);
				if (!r.success) return r;
				first = false;
				node = node->next;
			}
		}
		string_append_uchar(str, '}', a);
		break;
	}
	return new_success();
}

Result json_serialize(struct JSONValue *val, string *str, Allocator allocator) {
	string_new(str, "", allocator);
	Result r = json_serialize_rec(val, str, allocator);
	if (!r.success) {
		string_free(str, allocator);
		return r;
	}
	return new_success();
}