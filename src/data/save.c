#include "save.h"
#include "../json/document.h"
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

static Result set_wstring(JSONDocument *doc, JSONValue *object,
                          const char *key, const wchar_t *value) {
    string utf8;
    check(wchar_to_utf8_string(value, &utf8, ga));
    Result r = json_object_set_string_sv(doc, object, as_svc(key), as_sv(utf8));
    string_free(&utf8, ga);
    return r;
}

static Result convert_to_json(struct Veranstaltung *v_out, size_t v_count,
                              struct Modulgruppe *mg_out, size_t mg_count,
                              JSONDocument *doc) {
    json_document_set_object(doc);
    JSONValue *list;
    check(json_object_set_array(doc, &doc->value, "veranstaltungen", &list));
    for (size_t i = 0; i < v_count; i++) {
        struct Veranstaltung *v = &v_out[i];
        JSONValue *item;
        check(json_array_append_object(doc, list, &item));
        check(set_wstring(doc, item, "name", v->name));
        check(json_object_set_double(doc, item, "note", v->note));
        check(json_object_set_int(doc, item, "lp", v->lp));
        check(json_object_set_int(doc, item, "modulindex", v->modulgruppenindex));

        JSONValue *semester;
        check(json_object_set_object(doc, item, "semester", &semester));
        check(json_object_set_int(doc, semester, "jahr", v->semester.jahr));
        check(json_object_set_string(doc, semester, "jahreszeit",
                                    v->semester.jahreszeit == Winter ? "winter" : "sommer"));

        const char *state;
        switch (v->state) {
        case Bestanden: state = "bestanden"; break;
        case NichtBestanden: state = "nicht_bestanden"; break;
        case Ausstehend: state = "ausstehend"; break;
        default: return new_error("Invalid course state", ESaveFormatError);
        }
        check(json_object_set_string(doc, item, "state", state));
    }

    check(json_object_set_array(doc, &doc->value, "modulgruppen", &list));
    for (size_t i = 0; i < mg_count; i++) {
        struct Modulgruppe *mg = &mg_out[i];
        JSONValue *item;
        check(json_array_append_object(doc, list, &item));
        check(set_wstring(doc, item, "name", mg->name));
        check(json_object_set_int(doc, item, "modulgruppenindex", mg->modulgruppenindex));
        check(json_object_set_int(doc, item, "lp_todo", mg->lp_todo));
    }
    return new_success();
}

static Result serialize_save_data(struct Veranstaltung *v, size_t v_count,
                                  struct Modulgruppe *mg, size_t mg_count,
                                  string *out) {
    JSONDocument doc = json_document_new();
    Result r = convert_to_json(v, v_count, mg, mg_count, &doc);
    // Serialized output is separately owned and survives document destruction.
    if (r.success) r = json_document_serialize(&doc, out);
    json_document_free(&doc);
    return r;
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
