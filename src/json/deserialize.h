#include "utils/string/dstring.h"
#include "internal/jsonvalue.h"
#include "internal/parser.h"
#include "config.h"

/**
 * @brief Creates a new parser
 * @param config Parser configuration
 */
Parser json_parser_new(ParserConfig config);

/**
 * @brief Deserializes a JSONValue from a string using an existing parser.
 * @param parser Pointer to an initialized parser
 * @param json Pointer to the string containing the JSON data
 * @param result Pointer to store the resulting JSONValue
 */
Result json_parser_deserialize(Parser *parser, string *json, struct JSONValue *result);

/**
 * @brief Frees the parser
 * @param parser Pointer to the parser to free
 */
void json_parser_free(Parser *parser);

/**
 * @brief Frees a JSONValue produced by the parser so that the parser can be used again.
 * @param parser Parser that should be reset
 * @param value Pointer to the JSONValue to free
 */
void json_parser_value_free(Parser *parser, JSONValue *value);

/**
 * @brief Gets a field from a json object and verifies its type.
 * 
 * @param obj The JSON object
 * @param key The key to look for
 * @param type The expected type
 * @param out Pointer to store the resulting JSONValue pointer
 */
Result json_get_typed(const JSONValue *obj, string_view key, JSONType type, JSONValue *out);

/**
 * @brief Extracts a number field from a JSON object and verifies it is an integer.
 * @param obj The JSON object
 * @param key The key to look for
 * @param out Pointer to store the resulting integer
 */
Result extract_int(const JSONValue *obj, string_view key, int *out);
