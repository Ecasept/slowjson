#include "../json/utils/string/dstring.h"
#include "../json/utils/unicode/utf8.h"
#include "../json/utils/unicode/wchar.h"
#include "../midend/data.h"
#include "../json/deserialize.h"
#include "load.h"
#include "../json/utils/string/file.h"
#include "../json/config.h"
#include "../json/utils/alloc/default.h"

const char *JSON_SAVEFILE_NAME = "data.json";

/**
 * @brief Extracts a string field from a JSON object and converts it to wchar_t string.
 * @param obj The JSON object
 * @param key The key to look for
 * @param out Pointer to store the resulting wchar_t string (will be allocated)
 */
static Result extract_wstring(const JSONValue *obj, const char *key, wchar_t **out) {
    string_view val;
    check(json_object_get_string(obj, key, &val));
    return utf8_string_to_wchar(val, out, ga);
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
    string_view season_val;
    check(json_object_get_cint(val, "jahr", &sem->jahr));
    check(json_object_get_string(val, "jahreszeit", &season_val));

    if (sv_eq(season_val, svl("winter"))) {
        sem->jahreszeit = Winter;
    } else if (sv_eq(season_val, svl("sommer"))) {
        sem->jahreszeit = Sommer;
    } else {
        Result r = new_errorf("Invalid jahreszeit value: \"%.*s\"",
							  ESaveFormatError, (int)season_val.size,
							  season_val.data);
        return r;
    }
    return new_success();
}

static Result parse_modulgruppe(const JSONValue *val, struct Modulgruppe *mg) {
    memset(mg, 0, sizeof(struct Modulgruppe));
    Result r;
    if (!(r = extract_wstring(val, "name", &mg->name)).success) goto error;
    if (!(r = json_object_get_cint(val, "lp_todo", &mg->lp_todo)).success) goto error;
	if (!(r = json_object_get_cint(val, "modulgruppenindex", &mg->modulgruppenindex)).success) goto error;

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
    if (!(r = json_object_get_double(val, "note", &v->note)).success) goto error;

    if (!(r = json_object_get_cint(val, "lp", &v->lp)).success) goto error;
    if (!(r = json_object_get_cint(val, "modulindex", &v->modulgruppenindex)).success) goto error;

    if (!(r = json_object_get_object(val, "semester", &tmp)).success) goto error;
    if (!(r = parse_semester(&tmp, &v->semester)).success) goto error;

    string_view state;
    if (!(r = json_object_get_string(val, "state", &state)).success) goto error;
    if (sv_eq(state, svl("bestanden"))) v->state = Bestanden;
    else if (sv_eq(state, svl("nicht_bestanden"))) v->state = NichtBestanden;
    else if (sv_eq(state, svl("ausstehend"))) v->state = Ausstehend;
    else {
        r = new_errorf("Invalid state value: \"%.*s\"", ESaveFormatError, (int)state.size, state.data);
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
    if (!(r = json_object_get_array(&root, "veranstaltungen", &v_arr)).success) goto cleanup;
    if (!(r = json_object_get_array(&root, "modulgruppen", &mg_arr)).success) goto cleanup;

    // Load Veranstaltungen
    *v_count = v_arr.list.length;
    veranstaltung_list_init(&v_list, *v_count, ga);
    for (size_t i = 0; i < *v_count; i++) {
        struct Veranstaltung item;
        if (!(r = parse_veranstaltung(&v_arr.list.data[i], &item)).success) goto cleanup;
        veranstaltung_list_push(&v_list, item, ga);
    }

    // Load Modulgruppen
    *mg_count = mg_arr.list.length;
    modulgruppe_list_init(&mg_list, *mg_count, ga);
    for (size_t i = 0; i < *mg_count; i++) {
        struct Modulgruppe item;
        if (!(r = parse_modulgruppe(&mg_arr.list.data[i], &item)).success) goto cleanup;
        modulgruppe_list_push(&mg_list, item, ga);
    }

    *v = v_list.data;
    *mg = mg_list.data;
    return new_success();

cleanup:
    for (size_t i = 0; i < v_list.length; i++) free_veranstaltung_content(&v_list.data[i]);
    for (size_t i = 0; i < mg_list.length; i++) free_modulgruppe_content(&mg_list.data[i]);
    if (v_list.data) veranstaltung_list_free(&v_list, ga);
    if (mg_list.data) modulgruppe_list_free(&mg_list, ga);
    return r;
}


Result load_data_from_savefile(struct Veranstaltung **v, struct Modulgruppe **mg,
                         size_t *v_count, size_t *mg_count) {
    string json;
    Result r = read_file_to_string(JSON_SAVEFILE_NAME, &json);
    if (!r.success) return r;

    Parser parser = json_parser_new(config_default_parser_config());
    JSONDocument result = json_document_new();
    r = json_parser_deserialize(&parser, &json, &result);
    string_free(&json, ga);

    if (!r.success) {
        json_document_free(&result);
        return r;
    }

    r = parse_savefile_json(result.value, v, mg, v_count, mg_count);
	json_document_free(&result);
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
