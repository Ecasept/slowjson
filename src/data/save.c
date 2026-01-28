#include "save.h"
#include "../json/serialize.h"
#include "../json/utils/alloc/default.h"
#include "../json/utils/string/file.h"
#include "../json/utils/unicode/wchar.h"
#include "../midend/data.h"
#include "load.h"

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
	*root = json_value_new_object(ga);
	JSONValue mg_list = {0};

	JSONValue v_list = json_value_new_array(ga);
	for (size_t i = 0; i < v_count; i++) {
		struct Veranstaltung *v = &v_out[i];
		JSONValue veranstaltung = json_value_new_object(ga);

		string name;
		r = wchar_to_utf8_string(v->name, &name, ga);
		if (!r.success) {
			json_value_free(&veranstaltung, ga);
			json_value_free(&v_list, ga);
			goto cleanup;
		}
		json_value_hashmap_set(&veranstaltung.hashmap, string_newr("name", ga),
							   json_value_new_string(&name), ga);

		json_value_hashmap_set(&veranstaltung.hashmap, string_newr("note", ga),
							   json_value_new_float(v->note), ga);
		json_value_hashmap_set(&veranstaltung.hashmap, string_newr("lp", ga),
							   json_value_new_integer(v->lp), ga);
		json_value_hashmap_set(
			&veranstaltung.hashmap, string_newr("modulindex", ga),
			json_value_new_integer(v->modulgruppenindex), ga);
		// Semester
		JSONValue semester = json_value_new_object(ga);
		json_value_hashmap_set(&semester.hashmap, string_newr("jahr", ga),
							   json_value_new_integer(v->semester.jahr), ga);
		const char *jahreszeit_str = NULL;
		if (v->semester.jahreszeit == Winter) {
			jahreszeit_str = "winter";
		} else {
			jahreszeit_str = "sommer";
		}
		json_value_hashmap_set(&semester.hashmap, string_newr("jahreszeit", ga),
							   json_value_new_string_cstr(jahreszeit_str, ga), ga);
		json_value_hashmap_set(&veranstaltung.hashmap,
							   string_newr("semester", ga), semester, ga);

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
		json_value_hashmap_set(&veranstaltung.hashmap, string_newr("state", ga),
							   json_value_new_string_cstr(state_str, ga), ga);

		json_value_list_push(&v_list.list, veranstaltung, ga);
	}
	json_value_hashmap_set(&root->hashmap, string_newr("veranstaltungen", ga),
						   v_list, ga);

	mg_list = json_value_new_array(ga);
	for (size_t i = 0; i < mg_count; i++) {
		struct Modulgruppe *mg = &mg_out[i];
		JSONValue modulgruppe = json_value_new_object(ga);

		string name;
		r = wchar_to_utf8_string(mg->name, &name, ga);
		if (!r.success) {
			json_value_free(&modulgruppe, ga);
			json_value_free(&mg_list, ga);
			goto cleanup;
		}
		json_value_hashmap_set(&modulgruppe.hashmap, string_newr("name", ga),
							   json_value_new_string(&name), ga);

		json_value_hashmap_set(
			&modulgruppe.hashmap, string_newr("modulgruppenindex", ga),
			json_value_new_integer(mg->modulgruppenindex), ga);

		json_value_hashmap_set(&modulgruppe.hashmap, string_newr("lp_todo", ga),
							   json_value_new_integer(mg->lp_todo), ga);
		json_value_list_push(&mg_list.list, modulgruppe, ga);
	}
	json_value_hashmap_set(&root->hashmap, string_newr("modulgruppen", ga),
						   mg_list, ga);
	return new_success();

cleanup:
	json_value_free(root, ga);
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
	r = json_serialize(&root, out, ga);
	if (!r.success) {
		json_value_free(&root, ga);
		return r;
	}
	json_value_free(&root, ga);
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
	string_free(&str, ga);
	return r;
}
