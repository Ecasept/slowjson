#pragma once
#include "../midend/data.h"
#include "../json/utils/custom_error.h"

extern const char *JSON_SAVEFILE_NAME;

Result load_data_from_savefile(struct Veranstaltung **v, struct Modulgruppe **mg,
						 size_t *v_count, size_t *mg_count);


#define TYPE struct Veranstaltung
#define TYPED_NAME(name) veranstaltung_##name
#define LIST_DECLARATION
#include "../json/utils/list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME

#define TYPE struct Modulgruppe
#define TYPED_NAME(name) modulgruppe_##name
#define LIST_DECLARATION
#include "../json/utils/list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME
