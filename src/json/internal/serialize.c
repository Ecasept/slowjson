#include "../utils/hashmap/hashmap_node.h"
#include "../serialize.h"

void json_serialize(JSONValue *val, string *str) {
	switch (val->type) {
	case JSON_NULL:
		string_append_cstr(str, "null");
		break;
	case JSON_BOOL:
		string_append_cstr(str, val->boolean ? "true" : "false");
		break;
	case JSON_NUMBER:
		{
			char buf[64];
			if (val->number.is_integer) {
				sprintf(buf, "%lld", (long long)val->number.int_value);
			} else {
				sprintf(buf, "%f", val->number.float_value);
			}
			string_append_cstr(str, buf);
		}
		break;
	case JSON_STRING:
		string_append_uchar(str, '"');
		// A byte <= 0x7F may only appear representing a single-byte UTF-8 character
		// So we can iterate over the bytes directly instead of decoding codepoints
		for (size_t i = 0; i < val->str.arr.length; i++) {
			uchar c = val->str.arr.data[i];
			switch (c) {
			case '"':
				string_append_cstr(str, "\\\"");
				break;
			case '\n':
				string_append_cstr(str, "\\n");
				break;
			case '\t':
				string_append_cstr(str, "\\t");
				break;
			case '\\':
				string_append_cstr(str, "\\\\");
				break;
			case '\r':
				string_append_cstr(str, "\\r");
				break;
			case '\b':
				string_append_cstr(str, "\\b");
				break;
			case '\f':
				string_append_cstr(str, "\\f");
				break;
			default:
				if (c <= 0x1F) {
					// Control character
					char buf[7];
					sprintf(buf, "\\u%04X", c);
					string_append_cstr(str, buf);
				} else {
					string_append_uchar(str, c);
				}
				break;
			}
		}
		string_append_uchar(str, '"');
		break;
	case JSON_ARRAY:
		string_append_uchar(str, '[');
		for (size_t i = 0; i < val->list.length; i++) {
			if (i > 0) {string_append_uchar(str, ',');}
			json_serialize(&val->list.data[i], str);
		}
		string_append_uchar(str, ']');
		break;
	case JSON_OBJECT:
		string_append_uchar(str, '{');
		bool first = true;
		for (size_t i = 0; i < val->hashmap.buckets.length; i++) {
			json_value_hashmap_node *node = &val->hashmap.buckets.data[i];
			while (node && node->key.arr.data != NULL) {
				if (!first) string_append_uchar(str, ',');
				string_append_uchar(str, '"');
				string_append(str, &node->key);
				string_append_cstr(str, "\":");
				json_serialize(&node->value, str);
				first = false;
				node = node->next;
			}
		}
		string_append_uchar(str, '}');
		break;
	}
}
