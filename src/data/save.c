#include "../midend/data.h"
#include "../utils/unicode/wchar.h"
#include "../json/json.h"
#include "file.h"
#include "save.h"

/**
 * Example:
 * 
 * 
{
  "modulgruppen": [
    {
      "name": "abc",
      "lp_todo": 123
    }
  ],
"veranstaltungen": [
  {
    "name": "abc",
    "note": 123,
    "lp": 123,e
    "modulindex": 123,
    "semester": {
      "jahr": 123,
      "jahreszeit": "winter"
	  },
	"state": "bestanden"
  }
]
}
 */

static Result convert_to_json(struct Veranstaltung *v_out, size_t v_count,
						  struct Modulgruppe *mg_out, size_t mg_count,
						  JSONValue *root) {
	Result r;
	*root = json_value_new_object();
	JSONValue mg_list = {0};

	JSONValue v_list = json_value_new_array();
	for (size_t i = 0; i < v_count; i++) {
		struct Veranstaltung *v = &v_out[i];
		JSONValue veranstaltung = json_value_new_object();

		string name;
		r = wchar_to_utf8_string(v->name, &name);
		if (!r.success) {
			json_value_free(&veranstaltung);
			json_value_free(&v_list);
			goto cleanup;
		}
		json_value_hashmap_set(&veranstaltung.hashmap,
								 string_newr("name"),
								 json_value_new_string(&name));

		json_value_hashmap_set(&veranstaltung.hashmap,
								 string_newr("note"),
								 json_value_new_float(v->note));
		json_value_hashmap_set(&veranstaltung.hashmap,
								 string_newr("lp"),
								 json_value_new_integer(v->lp));
		json_value_hashmap_set(&veranstaltung.hashmap,
								 string_newr("modulindex"),
								 json_value_new_integer(v->modulgruppenindex));
		// Semester
		JSONValue semester = json_value_new_object();
		json_value_hashmap_set(&semester.hashmap,
								 string_newr("jahr"),
								 json_value_new_integer(v->semester.jahr));
		const char *jahreszeit_str = NULL;
		if (v->semester.jahreszeit == Winter) {
			jahreszeit_str = "winter";
		} else {
			jahreszeit_str = "sommer";
		}
		json_value_hashmap_set(&semester.hashmap,
								string_newr("jahreszeit"),
								json_value_new_string_cstr(jahreszeit_str));
		json_value_hashmap_set(&veranstaltung.hashmap,
								string_newr("semester"),
								semester);

		// State
		const char *state_str = NULL;
		switch (v->state) {
			case Bestanden:
				state_str = "bestanden";
				break;
			case NichtBestanden:
				state_str = "nicht_bestanden";
				break;
			case Ausstehend:
				state_str = "ausstehend";
				break;
		}
		json_value_hashmap_set(&veranstaltung.hashmap,
								string_newr("state"),
								json_value_new_string_cstr(state_str));

		json_value_list_push(&v_list.list, veranstaltung);
	}
	json_value_hashmap_set(&root->hashmap,
							string_newr("veranstaltungen"),
							v_list);

	mg_list = json_value_new_array();
	for (size_t i = 0; i < mg_count; i++) {
		struct Modulgruppe *mg = &mg_out[i];
		JSONValue modulgruppe = json_value_new_object();


		string name;
		r = wchar_to_utf8_string(mg->name, &name);
		if (!r.success) {
			json_value_free(&modulgruppe);
			json_value_free(&mg_list);
			goto cleanup;
		}
		json_value_hashmap_set(&modulgruppe.hashmap,
							   string_newr("name"),
							   json_value_new_string(&name));
		
	json_value_hashmap_set(&modulgruppe.hashmap,
							   string_newr("modulgruppenindex"),
							   json_value_new_integer(mg->modulgruppenindex));

							   json_value_hashmap_set(&modulgruppe.hashmap,
							   string_newr("lp_todo"),
							   json_value_new_integer(mg->lp_todo));
		json_value_list_push(&mg_list.list, modulgruppe);
	}
	json_value_hashmap_set(&root->hashmap,
							string_newr("modulgruppen"),
							mg_list);
	return new_success();

cleanup:
	json_value_free(root);
	return r;
}



static Result serialize_save_data(struct Veranstaltung *v, size_t v_count,
							   struct Modulgruppe *mg, size_t mg_count,
							   string *out) {
	JSONValue root;
	Result r = convert_to_json(v, v_count, mg, mg_count, &root);
	if (!r.success) {
		return r;
	}
	string_new(out, "");
	serialize_json(&root, out);
	json_value_free(&root);
	return new_success();
}

Result save_data_to_savefile(struct Veranstaltung *v, size_t v_count,
				 struct Modulgruppe *mg, size_t mg_count) {
	string str;
	Result r = serialize_save_data(v, v_count, mg, mg_count, &str);
	if (!r.success) {
		return r;
	}
	r = write_string_to_file(JSON_SAVEFILE_NAME, &str);
	string_free(&str);
	return r;
}
