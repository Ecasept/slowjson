#include "parser.h"
#include "lexer.h"
#include "../deserialize.h"
#include <stdlib.h>
#include "../utils/alloc/default.h"


// Forward declaration
static Result parse_json_value(Parser *parser,
							   JSONValue *out_value, size_t depth);

static Result get_tokens(Lexer *lexer, json_token_list *tokens) {
	JSONToken token;
	Result r;
	while (1) {
		r = lexer_next_token(lexer, &token);
		if (!r.success) {
			// Free previously allocated tokens
			for (size_t i = 0; i < tokens->length; i++) {
				token = json_token_list_get_unchecked(tokens, i);
				lexer_free_token(lexer, &token);
			}
			json_token_list_free(tokens, ga);
			return r;
		}
		json_token_list_push(tokens, token, ga);
		if (token.type == JSONTok_EOF) {
			break;
		}
	}
	return new_success();
}

static Result parser_peek_token_count(json_token_list *tokens, size_t position,
									  JSONToken *out_token, size_t *count) {
	if (position >= tokens->length) {
		return new_error("Unexpected end of input", EParserUnexpectedEOF);
	}
	(*count)++;
	*out_token = json_token_list_get_unchecked(tokens, position);
	if (out_token->type == JSONTok_Whitespace) {
		// Skip whitespace
		return parser_peek_token_count(tokens, position + 1, out_token, count);
	}
	return new_success();
}

static Result parser_peek_token(Parser *parser,
								JSONToken *out_token) {
	size_t count = 0;
	return parser_peek_token_count(parser->tokens, parser->position, out_token, &count);
}

static Result parser_consume_token(Parser *parser, JSONToken *out_token) {
	size_t count = 0;
	Result r = parser_peek_token_count(parser->tokens, parser->position, out_token, &count);
	if (!r.success) {
		return r;
	}
	parser->position += count;
	return new_success();
}
static Result parser_expect_token(Parser *parser,
								  JSONTokenType expected_type,
								  JSONToken *out_token) {
	JSONToken token;
	Result r = parser_consume_token(parser, &token);
	if (!r.success) {
		return r;
	}
	if (token.type != expected_type) {
		return new_errorf(
			"Unexpected token: expected %s but got %s at line %zu, column %zu",
			EParserSyntaxError, tk_as_str(expected_type), tk_as_str(token.type),
			token.line, token.column);
	}
	*out_token = token;
	return new_success();
}

static Result parse_json_array(Parser *parser, JSONValue *out_value, size_t depth) {
	Result r;
	JSONToken token;
	out_value->type = JSON_ARRAY;
	json_value_list_init(&out_value->list, 0, ga);

	r = parser_peek_token(parser, &token);
	if (!r.success)
		goto error;
	if (token.type == JSONTok_RBracket) {
		// Empty array
		r = parser_consume_token(parser, &token);
		if (!r.success)
			goto error;
		return new_success();
	}
	// Parse elements
	while (1) {
		JSONValue element;
		r = parse_json_value(parser, &element, depth + 1);
		if (!r.success)
			goto error;
		json_value_list_push(&out_value->list, element, ga);

		// Check for ',' or ']'
		r = parser_peek_token(parser, &token);
		if (!r.success)
			goto error;
		if (token.type == JSONTok_Comma) {
			r = parser_consume_token(parser, &token);
			if (!r.success)
				goto error;
		} else if (token.type == JSONTok_RBracket) {
			r = parser_consume_token(parser, &token);
			if (!r.success)
				goto error;
			break;
		} else {
			r = new_errorf(
				"Expected ',' or ']' in array, got %s at line %zu, column %zu",
				EParserSyntaxError, tk_as_str(token.type), token.line,
				token.column);
			goto error;
		}
	}
	return new_success();

error:
	for (size_t i = 0; i < out_value->list.length; i++) {
		json_value_free(&out_value->list.data[i], ga);
	}
	json_value_list_free(&out_value->list, ga);
	return r;
}

static Result parse_json_object(Parser *parser, JSONValue *out_value, size_t depth) {
	JSONToken token;
	Result r;
	out_value->type = JSON_OBJECT;
	json_value_hashmap_init(&out_value->hashmap, ga);

	r = parser_peek_token(parser, &token);
	if (!r.success)
		goto error;
	if (token.type == JSONTok_RBrace) {
		// Empty object
		r = parser_consume_token(parser, &token);
		if (!r.success)
			goto error;
		return new_success();
	}
	// Parse key-value pairs
	while (1) {
		// Parse key-value pair
		JSONToken key;
		r = parser_expect_token(parser, JSONTok_String, &key);
		if (!r.success)
			goto error;
		JSONValue value;
		r = parser_expect_token(parser, JSONTok_Colon, &token);
		if (!r.success)
			goto error;
		r = parse_json_value(parser, &value, depth + 1);
		if (!r.success)
			goto error;

		string key_clone;
		string_clone(&key.value, &key_clone, ga);
		json_value_hashmap_set(&out_value->hashmap, key_clone, value, ga);

		// Check for ',' or '}'
		r = parser_peek_token(parser, &token);
		if (!r.success)
			goto error;
		if (token.type == JSONTok_Comma) {
			r = parser_consume_token(parser, &token);
			if (!r.success)
				goto error;
		} else if (token.type == JSONTok_RBrace) {
			r = parser_consume_token(parser, &token);
			if (!r.success)
				goto error;
			break;
		} else {
			r = new_errorf(
				"Expected ',' or '}' in object, got %s at line %zu, column %zu",
				EParserSyntaxError, tk_as_str(token.type), token.line,
				token.column);
			goto error;
		}
	}
	return new_success();

error:
	json_value_hashmap_free(&out_value->hashmap, ga);
	return r;
}

static Result parse_json_number(Parser *parser, JSONValue *out_value) {
	JSONToken token;
	Result r = parser_expect_token(parser, JSONTok_Number, &token);
	if (!r.success) {
		return r;
	}
	out_value->type = JSON_NUMBER;
	out_value->number = token.number;
	return new_success();
}

static Result parse_json_value(Parser *parser, JSONValue *out_value, size_t depth) {
	if (depth > parser->config.limits.max_nesting_depth) {
		return new_errorf(
			"Exceeded maximum nesting depth of %zu",
			EDepthLimitExceeded, parser->config.limits.max_nesting_depth);
	}
	
	JSONToken token;
	check(parser_peek_token(parser, &token));
	switch (token.type) {
	case JSONTok_Number:
		return parse_json_number(parser, out_value);
	case JSONTok_String:
		out_value->type = JSON_STRING;
		string_new(&out_value->str, "", ga);
		string_append(&out_value->str, &token.value, ga);
		check(parser_consume_token(parser, &token));
		return new_success();
	case JSONTok_True:
		out_value->type = JSON_BOOL;
		out_value->boolean = true;
		check(parser_consume_token(parser, &token));
		return new_success();
	case JSONTok_False:
		out_value->type = JSON_BOOL;
		out_value->boolean = false;
		check(parser_consume_token(parser, &token));
		return new_success();
	case JSONTok_Null:
		out_value->type = JSON_NULL;
		check(parser_consume_token(parser, &token));
		return new_success();
	case JSONTok_LBracket:
		check(parser_consume_token(parser, &token));
		return parse_json_array(parser, out_value, depth);
	case JSONTok_LBrace:
		check(parser_consume_token(parser, &token));
		return parse_json_object(parser, out_value, depth);
	default:
		return new_errorf(
			"Unexpected token: encountered %s at line %zu, column %zu (expected value)",
			EParserSyntaxError, tk_as_str(token.type), token.line,
			token.column);
	}
}

static Result parse_json_value_top_level(Parser *parser, JSONValue *out_value) {
	Result r = parse_json_value(parser, out_value, 0);
	if (!r.success) {
		return r;
	}
	// Expect EOF
	JSONToken token;
	r = parser_expect_token(parser, JSONTok_EOF, &token);
	if (!r.success) {
		json_value_free(out_value, ga);
		return r;
	}
	return new_success();
}

Parser json_parser_new(ParserConfig config) {
	Parser parser = {0};
	parser.config = config;
	parser.arena = malloc(sizeof(Arena));
	if (parser.arena == NULL) {
		panic("Failed to allocate memory for parser arena");
	}
	*parser.arena = new_arena();
	parser.allocator = arena_as_allocator(parser.arena);
	return parser;
}

Result json_parser_deserialize(Parser *parser, string *json, JSONValue *result) {
	Lexer lexer;
	lexer_init(&lexer, json, parser->config, ga);
	json_token_list tokens;
	json_token_list_init(&tokens, 0, ga);
	Result r = get_tokens(&lexer, &tokens);
	if (!r.success) {
		return r;
	}

	parser->tokens = &tokens;
	parser->position = 0;
	r = parse_json_value_top_level(parser, result);

	// Free tokens
	for (size_t i = 0; i < parser->tokens->length; i++) {
		JSONToken token = json_token_list_get_unchecked(parser->tokens, i);
		lexer_free_token(&lexer, &token);
	}
	json_token_list_free(parser->tokens, ga);
	parser->tokens = NULL;
	return r;
}

void json_parser_free(Parser *parser) {
	parser->tokens = NULL;
	parser->position = 0;
	free(parser->arena);
}

void json_parser_value_free(Parser *parser, JSONValue *value) {
	json_value_free(value, ga);
	arena_free(parser->arena);
}

#define TYPE JSONToken
#define TYPED_NAME(name) json_token_##name
#define LIST_IMPLEMENTATION
#include "../utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
