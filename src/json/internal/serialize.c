#include "../utils/hashmap/hashmap_node.h"
#include "../serialize.h"
#include <math.h>
#include "../utils/alloc/default.h"


static Result json_serialize_rec(JSONValue *val, string *str);

static Result json_serialize_rec(JSONValue *val, string *str) {
	switch (val->type) {
	case JSON_NULL:
		string_append_cstr(str, "null", ga);
		break;
	case JSON_BOOL:
		string_append_cstr(str, val->boolean ? "true" : "false", ga);
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
			string_append_cstr(str, buf, ga);
		}
		break;
	case JSON_STRING:
		string_append_uchar(str, '"', ga);
		// A byte <= 0x7F may only appear representing a single-byte UTF-8 character
		// So we can iterate over the bytes directly instead of decoding codepoints
		for (size_t i = 0; i < val->str.arr.length; i++) {
			uchar c = val->str.arr.data[i];
			switch (c) {
			case '"':
				string_append_cstr(str, "\\\"", ga);
				break;
			case '\n':
				string_append_cstr(str, "\\n", ga);
				break;
			case '\t':
				string_append_cstr(str, "\\t", ga);
				break;
			case '\\':
				string_append_cstr(str, "\\\\", ga);
				break;
			case '\r':
				string_append_cstr(str, "\\r", ga);
				break;
			case '\b':
				string_append_cstr(str, "\\b", ga);
				break;
			case '\f':
				string_append_cstr(str, "\\f", ga);
				break;
			default:
				if (c <= 0x1F) {
					// Control character
					char buf[7];
					sprintf(buf, "\\u%04X", c);
					string_append_cstr(str, buf, ga);
				} else {
					string_append_uchar(str, c, ga);
				}
				break;
			}
		}
		string_append_uchar(str, '"', ga);
		break;
	case JSON_ARRAY:
		string_append_uchar(str, '[', ga);
		for (size_t i = 0; i < val->list.length; i++) {
			if (i > 0) {string_append_uchar(str, ',', ga);}
			Result r = json_serialize_rec(&val->list.data[i], str);
			if (!r.success) return r;
		}
		string_append_uchar(str, ']', ga);
		break;
	case JSON_OBJECT:
		string_append_uchar(str, '{', ga);
		bool first = true;
		for (size_t i = 0; i < val->hashmap.buckets.length; i++) {
			json_value_hashmap_node *node = &val->hashmap.buckets.data[i];
			while (node && node->key.arr.data != NULL) {
				if (!first) string_append_uchar(str, ',', ga);
				string_append_uchar(str, '"', ga);
				string_append(str, &node->key, ga);
				string_append_cstr(str, "\":", ga);
				Result r = json_serialize_rec(&node->value, str);
				if (!r.success) return r;
				first = false;
				node = node->next;
			}
		}
		string_append_uchar(str, '}', ga);
		break;
	}
	return new_success();
}

Result json_serialize(struct JSONValue *val, string *str) {
	string_new(str, "", ga);
	Result r = json_serialize_rec(val, str);
	if (!r.success) {
		string_free(str, ga);
		return r;
	}
	return new_success();
}
