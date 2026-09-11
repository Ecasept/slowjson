#pragma once
#include "document.h"
#include "internal/parser.h"
#include "config.h"

Parser json_parser_new(ParserConfig config);
/* doc must be initialized. Parsing replaces its root; failure leaves a null
 * root and a reusable document. Input can be freed after this call. */
Result json_parser_deserialize(Parser *parser, string *json, JSONDocument *doc);
