#include "../json/utils/string/dstring.h"
#include "../json/utils/unicode/utf8.h"
#include "../json/utils/unicode/wchar.h"
#include "../midend/data.h"
#include "../json/deserialize.h"
#include "load.h"
#include "../json/utils/string/file.h"
#include "../json/config.h"

const char *JSON_SAVEFILE_NAME = "data.json";

/**
 * @brief Extracts a string field from a JSON object and converts it to wchar_t string.
 * @param obj The JSON object
 * @param key The key to look for
 * @param out Pointer to store the resulting wchar_t string (will be allocated)
 */
static Result extract_wstring(const JSONValue *obj, string_view key, wchar_t **out) {
    JSONValue val;
    check(json_get_typed(obj, key, JSON_STRING, &val));
    return utf8_string_to_wchar(as_sv(val.str), out);
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
    check(extract_int(val, svl("jahr"), &sem->jahr));
    check(json_get_typed(val, svl("jahreszeit"), JSON_STRING, &season_val));

    if (string_eq_cstr(&season_val.str, "winter")) {
        sem->jahreszeit = Winter;
    } else if (string_eq_cstr(&season_val.str, "sommer")) {
        sem->jahreszeit = Sommer;
    } else {
        Result r = new_errorf("Invalid jahreszeit value: \"%.*s\"",
							  ESaveFormatError, (int)season_val.str.arr.length,
							  season_val.str.arr.data);
        return r;
    }
    return new_success();
}

static Result parse_modulgruppe(const JSONValue *val, struct Modulgruppe *mg) {
    memset(mg, 0, sizeof(struct Modulgruppe));
    Result r;
    if (!(r = extract_wstring(val, svl("name"), &mg->name)).success) goto error;
    if (!(r = extract_int(val, svl("lp_todo"), &mg->lp_todo)).success) goto error;
	if (!(r = extract_int(val, svl("modulgruppenindex"), &mg->modulgruppenindex)).success) goto error;

    return new_success();
error:
    free_modulgruppe_content(mg);
    return r;
}

static Result parse_veranstaltung(const JSONValue *val, struct Veranstaltung *v) {
    memset(v, 0, sizeof(struct Veranstaltung));
    Result r;
    JSONValue tmp;

    if (!(r = extract_wstring(val, svl("name"), &v->name)).success) goto error;
    
    // Note can be int or float
    if (!(r = json_get_typed(val, svl("note"), JSON_NUMBER, &tmp)).success) goto error;
    v->note = tmp.number.is_integer ? (double)tmp.number.int_value : tmp.number.float_value;

    if (!(r = extract_int(val, svl("lp"), &v->lp)).success) goto error;
    if (!(r = extract_int(val, svl("modulindex"), &v->modulgruppenindex)).success) goto error;

    if (!(r = json_get_typed(val, svl("semester"), JSON_OBJECT, &tmp)).success) goto error;
    if (!(r = parse_semester(&tmp, &v->semester)).success) goto error;

    if (!(r = json_get_typed(val, svl("state"), JSON_STRING, &tmp)).success) goto error;
    if (string_eq_cstr(&tmp.str, "bestanden")) v->state = Bestanden;
    else if (string_eq_cstr(&tmp.str, "nicht_bestanden")) v->state = NichtBestanden;
    else if (string_eq_cstr(&tmp.str, "ausstehend")) v->state = Ausstehend;
    else {
        r = new_errorf("Invalid state value: \"%.*s\"", ESaveFormatError, (int)tmp.str.arr.length, tmp.str.arr.data);
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
    if (!(r = json_get_typed(&root, svl("veranstaltungen"), JSON_ARRAY, &v_arr)).success) goto cleanup;
    if (!(r = json_get_typed(&root, svl("modulgruppen"), JSON_ARRAY, &mg_arr)).success) goto cleanup;

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
    return new_success();

cleanup:
    for (size_t i = 0; i < v_list.length; i++) free_veranstaltung_content(&v_list.data[i]);
    for (size_t i = 0; i < mg_list.length; i++) free_modulgruppe_content(&mg_list.data[i]);
    if (v_list.data) veranstaltung_list_free(&v_list);
    if (mg_list.data) modulgruppe_list_free(&mg_list);
    return r;
}


Result load_data_from_savefile(struct Veranstaltung **v, struct Modulgruppe **mg,
                         size_t *v_count, size_t *mg_count) {
    string json;
    Result r = read_file_to_string(JSON_SAVEFILE_NAME, &json);
    if (!r.success) return r;

    Parser parser = json_parser_new(config_default_parser_config());
    JSONValue root;
    r = json_parser_deserialize(&parser, &json, &root);
    string_free(&json);

    if (!r.success) {
		json_parser_value_free(&parser, &root);
        json_parser_free(&parser);
        return r;
    }

    r = parse_savefile_json(root, v, mg, v_count, mg_count);
	json_parser_value_free(&parser, &root);
	json_parser_free(&parser);
    return r;
}

#define TYPE struct Veranstaltung
#define TYPED_NAME(name) veranstaltung_##name
#define LIST_IMPLEMENTATION
#include "../json/utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME

#define TYPE struct Modulgruppe
#define TYPED_NAME(name) modulgruppe_##name
#define LIST_IMPLEMENTATION
#include "../json/utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
