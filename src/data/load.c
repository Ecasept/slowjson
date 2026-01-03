#include "../utils/dstring.h"
#include "../utils/unicode/utf8.h"
#include "../utils/unicode/wchar.h"
#include "../midend/data.h"
#include "../json/json.h"
#include "load.h"
#include "file.h"
/**
 * @brief Gets a field from a json object and verifies its type.
 * 
 * @param obj The JSON object
 * @param key The key to look for
 * @param type The expected type
 * @param out Pointer to store the resulting JSONValue pointer
 */
static Result json_get_typed(const JSONValue *obj, const char *key, JSONType type, JSONValue *out) {
    if (obj->type != JSON_OBJECT) {
        return new_error("Expected JSON object", ESaveFormatError);
    }
    
	check(json_value_hashmap_get_cstr(&obj->hashmap, key, out));
	
    if (out->type != type) {
        return new_errorf("Field '%s': expected %s, got %s",
                          ESaveFormatError, key, jtostr(type), jtostr(out->type));
    }
    return new_success();
}

static Result extract_int(const JSONValue *obj, const char *key, int *out) {
    JSONValue val;
    check(json_get_typed(obj, key, JSON_NUMBER, &val));
    if (!val.number.is_integer) {
        return new_errorf("Field '%s' must be an integer", ESaveFormatError, key);
    }
    *out = (int)val.number.int_value;
    return new_success();
}

static Result extract_wstring(const JSONValue *obj, const char *key, wchar_t **out) {
    JSONValue val;
    check(json_get_typed(obj, key, JSON_STRING, &val));
    return utf8_string_to_wchar(&val.str, out);
}

static void free_veranstaltung_content(struct Veranstaltung *v) {
    if (v->name) {
        free(v->name);
        v->name = NULL;
    }
}

static void free_modulgruppe_content(struct Modulgruppe *mg) {
    if (mg->name) {
        free(mg->name);
        mg->name = NULL;
    }
}

static Result parse_semester(const JSONValue *val, struct Semester *sem) {
    JSONValue season_val;
    check(extract_int(val, "jahr", &sem->jahr));
    check(json_get_typed(val, "jahreszeit", JSON_STRING, &season_val));

    if (string_eq_cstr(&season_val.str, "winter")) {
        sem->jahreszeit = Winter;
    } else if (string_eq_cstr(&season_val.str, "sommer")) {
        sem->jahreszeit = Sommer;
    } else {
		char *cstr;
		string_to_cstr(&season_val.str, &cstr);
        Result r = new_errorf("Invalid jahreszeit value: \"%s\"", ESaveFormatError, cstr);
        free(cstr);
        return r;
    }
    return new_success();
}

static Result parse_modulgruppe(const JSONValue *val, struct Modulgruppe *mg) {
    memset(mg, 0, sizeof(struct Modulgruppe));
    Result r;
    
    if (!(r = extract_wstring(val, "name", &mg->name)).success) goto error;
    if (!(r = extract_int(val, "lp_todo", &mg->lp_todo)).success) goto error;
	if (!(r = extract_int(val, "modulgruppenindex", &mg->modulgruppenindex)).success) goto error;

    return new_success();
error:
    free_modulgruppe_content(mg);
    return r;
}

static Result parse_veranstaltung(const JSONValue *val, struct Veranstaltung *v) {
    memset(v, 0, sizeof(struct Veranstaltung));
    Result r;
    JSONValue tmp;

    if (!(r = extract_wstring(val, "name", &v->name)).success) goto error;
    
    // Note can be int or float
    if (!(r = json_get_typed(val, "note", JSON_NUMBER, &tmp)).success) goto error;
    v->note = tmp.number.is_integer ? (double)tmp.number.int_value : tmp.number.float_value;

    if (!(r = extract_int(val, "lp", &v->lp)).success) goto error;
    if (!(r = extract_int(val, "modulindex", &v->modulgruppenindex)).success) goto error;
    
    if (!(r = json_get_typed(val, "semester", JSON_OBJECT, &tmp)).success) goto error;
    if (!(r = parse_semester(&tmp, &v->semester)).success) goto error;

    if (!(r = json_get_typed(val, "state", JSON_STRING, &tmp)).success) goto error;
    if (string_eq_cstr(&tmp.str, "bestanden")) v->state = Bestanden;
    else if (string_eq_cstr(&tmp.str, "nicht_bestanden")) v->state = NichtBestanden;
    else if (string_eq_cstr(&tmp.str, "ausstehend")) v->state = Ausstehend;
    else {
		char *cstr;
		string_to_cstr(&tmp.str, &cstr);
        r = new_errorf("Invalid state value: \"%s\"", ESaveFormatError, cstr);
        free(cstr);
        goto error;
    }

    return new_success();
error:
    free_veranstaltung_content(v);
    return r;
}

static Result parse_savefile_json(JSONValue root, struct Veranstaltung **v, struct Modulgruppe **mg,
                               size_t *v_count, size_t *mg_count) {



    JSONValue v_arr, mg_arr;
    veranstaltung_list v_list = {0};
    modulgruppe_list mg_list = {0};
	Result r;

    // Use TRY for logic that doesn't require cleanup yet
    if (!(r = json_get_typed(&root, "veranstaltungen", JSON_ARRAY, &v_arr)).success) goto cleanup;
    if (!(r = json_get_typed(&root, "modulgruppen", JSON_ARRAY, &mg_arr)).success) goto cleanup;

    // Load Veranstaltungen
    *v_count = v_arr.list.length;
    veranstaltung_list_init(&v_list, *v_count);
    for (size_t i = 0; i < *v_count; i++) {
        struct Veranstaltung item;
        if (!(r = parse_veranstaltung(&v_arr.list.data[i], &item)).success) goto cleanup;
        veranstaltung_list_push(&v_list, item);
    }

    // Load Modulgruppen
    *mg_count = mg_arr.list.length;
    modulgruppe_list_init(&mg_list, *mg_count);
    for (size_t i = 0; i < *mg_count; i++) {
        struct Modulgruppe item;
        if (!(r = parse_modulgruppe(&mg_arr.list.data[i], &item)).success) goto cleanup;
        modulgruppe_list_push(&mg_list, item);
    }

    *v = v_list.data;
    *mg = mg_list.data;
    json_value_free(&root);
    return new_success();

cleanup:
    for (size_t i = 0; i < v_list.length; i++) free_veranstaltung_content(&v_list.data[i]);
    for (size_t i = 0; i < mg_list.length; i++) free_modulgruppe_content(&mg_list.data[i]);
    if (v_list.data) veranstaltung_list_free(&v_list);
    if (mg_list.data) modulgruppe_list_free(&mg_list);
    json_value_free(&root);
    return r;
}


Result load_data_from_savefile(struct Veranstaltung **v, struct Modulgruppe **mg,
                         size_t *v_count, size_t *mg_count) {
    string json;
    Result r = read_file_to_string(JSON_SAVEFILE_NAME, &json);
    if (!r.success) return r;

    JSONValue root;
    r = deserialize_json(&json, &root);
    string_free(&json);
    
    if (!r.success) return r;

    return parse_savefile_json(root, v, mg, v_count, mg_count);
}

#define TYPE struct Veranstaltung
#define TYPED_NAME(name) veranstaltung_##name
#define LIST_IMPLEMENTATION
#include "../utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME

#define TYPE struct Modulgruppe
#define TYPED_NAME(name) modulgruppe_##name
#define LIST_IMPLEMENTATION
#include "../utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
