#include "serialize.h"
#include "../hashmap/hashmap_node.h"

void serialize_json(JSONValue *val, string *str) {
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
		string_append(str, &val->str);
		string_append_uchar(str, '"');
		break;
	case JSON_ARRAY:
		string_append_uchar(str, '[');
		for (size_t i = 0; i < val->list.length; i++) {
			if (i > 0) {string_append_uchar(str, ',');}
			serialize_json(&val->list.data[i], str);
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
				serialize_json(&node->value, str);
				first = false;
				node = node->next;
			}
		}
		string_append_uchar(str, '}');
		break;
	}
}
