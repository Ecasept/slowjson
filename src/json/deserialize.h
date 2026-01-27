#pragma once
#include "utils/string/dstring.h"
#include "internal/jsonvalue.h"
#include "internal/parser.h"
#include "config.h"

struct ParserResult {
    JSONValue value;
    Allocator parser_allocator;
    Arena *parser_arena;
    Allocator lexer_allocator;
    Arena *lexer_arena;
};
typedef struct ParserResult ParserResult;

/**
 * @brief Creates a new parser
 * @param config Parser configuration
 */
Parser json_parser_new(ParserConfig config);

/**
 * @brief Deserializes a JSONValue from a string using an existing parser.
 * @param parser Pointer to an initialized parser
 * @param json Pointer to the string containing the JSON data
 * @param result Pointer to store the resulting ParserResult
 */
Result json_parser_deserialize(Parser *parser, string *json, ParserResult *result);

/**
 * @brief Frees a ParserResult produced by the parser.
 * @param result Pointer to the ParserResult to free
 */
void parser_result_free(ParserResult *result);

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
