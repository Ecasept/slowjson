#include "parser.h"
#include "lexer.h"
#include "../json.h"

// Forward declaration
static Result parse_json_value(json_token_list *tokens, size_t *position,
							   JSONValue *out_value);

static Result get_tokens(Lexer *lexer, json_token_list *tokens) {
	JSONToken token;
	Result r;
	while (1) {
		r = lexer_next_token(lexer, &token);
		if (!r.success) {
			// Free previously allocated tokens
			for (size_t i = 0; i < tokens->length; i++) {
				json_token_list_get(tokens, i, &token);
				lexer_free_token(&token);
			}
			json_token_list_free(tokens);
			return r;
		}
		json_token_list_push(tokens, token);
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
	json_token_list_get(tokens, position, out_token);
	if (out_token->type == JSONTok_Whitespace) {
		// Skip whitespace
		return parser_peek_token_count(tokens, position + 1, out_token, count);
	}
	return new_success();
}

static Result parser_peek_token(json_token_list *tokens, size_t position,
								JSONToken *out_token) {
	size_t count = 0;
	return parser_peek_token_count(tokens, position, out_token, &count);
}

static Result parser_consume_token(json_token_list *tokens, size_t *position,
								   JSONToken *out_token) {

	size_t count = 0;
	Result r = parser_peek_token_count(tokens, *position, out_token, &count);
	if (!r.success) {
		return r;
	}
	*position += count;
	return new_success();
}
static Result parser_expect_token(json_token_list *tokens, size_t *position,
								  JSONTokenType expected_type,
								  JSONToken *out_token) {
	JSONToken token;
	Result r = parser_consume_token(tokens, position, &token);
	if (!r.success) {
		return r;
	}
	if (token.type != expected_type) {
		return new_errorf(
			"Expected token %s but got %s at line %zu, column %zu",
			EParserSyntaxError, tk_as_str(expected_type), tk_as_str(token.type),
			token.line, token.column);
	}
	*out_token = token;
	return new_success();
}

static Result parser_check_token(json_token_list *tokens, size_t position,
								 JSONTokenType expected_type, bool *out_match) {
	JSONToken token;
	Result r = parser_peek_token(tokens, position, &token);
	if (!r.success) {
		return r;
	}
	*out_match = (token.type == expected_type);
	return new_success();
}

static Result parse_json_array(json_token_list *tokens, size_t *position,
							   JSONValue *out_value) {
	Result r;
	JSONToken token;
	out_value->type = JSON_ARRAY;
	json_value_list_init(&out_value->list, 0);
	// Consume '['
	r = parser_expect_token(tokens, position, JSONTok_LBracket, &token);
	if (!r.success)
		goto error;

	r = parser_peek_token(tokens, *position, &token);
	if (!r.success)
		goto error;
	if (token.type == JSONTok_RBracket) {
		// Empty array
		r = parser_consume_token(tokens, position, &token);
		if (!r.success)
			goto error;
		return new_success();
	}
	// Parse elements
	while (1) {
		JSONValue element;
		r = parse_json_value(tokens, position, &element);
		if (!r.success)
			goto error;
		json_value_list_push(&out_value->list, element);

		// Check for ',' or ']'
		r = parser_peek_token(tokens, *position, &token);
		if (!r.success)
			goto error;
		if (token.type == JSONTok_Comma) {
			r = parser_consume_token(tokens, position, &token);
			if (!r.success)
				goto error;
		} else if (token.type == JSONTok_RBracket) {
			r = parser_consume_token(tokens, position, &token);
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
	json_value_list_free(&out_value->list);
	return r;
}

static Result parse_json_object(json_token_list *tokens, size_t *position,
								JSONValue *out_value) {
	JSONToken token;
	Result r;
	out_value->type = JSON_OBJECT;
	json_value_hashmap_init(&out_value->hashmap);
	// Consume '{'
	r = parser_expect_token(tokens, position, JSONTok_LBrace, &token);
	if (!r.success) {
		panic("JSON object called but no '{' found");
	}

	r = parser_peek_token(tokens, *position, &token);
	if (!r.success)
		goto error;
	if (token.type == JSONTok_RBrace) {
		// Empty object
		r = parser_consume_token(tokens, position, &token);
		if (!r.success)
			goto error;
		return new_success();
	}
	// Parse key-value pairs
	while (1) {
		// Parse key-value pair
		JSONToken key;
		r = parser_expect_token(tokens, position, JSONTok_String, &key);
		if (!r.success)
			goto error;
		JSONValue value;
		r = parser_expect_token(tokens, position, JSONTok_Colon, &token);
		if (!r.success)
			goto error;
		r = parse_json_value(tokens, position, &value);
		if (!r.success)
			goto error;

		string key_clone;
		string_clone(&key.value, &key_clone);
		json_value_hashmap_set(&out_value->hashmap, key_clone, value);

		// Check for ',' or '}'
		r = parser_peek_token(tokens, *position, &token);
		if (!r.success)
			goto error;
		if (token.type == JSONTok_Comma) {
			r = parser_consume_token(tokens, position, &token);
			if (!r.success)
				goto error;
		} else if (token.type == JSONTok_RBrace) {
			r = parser_consume_token(tokens, position, &token);
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
	json_value_hashmap_free(&out_value->hashmap);
	return r;
}

static Result parse_json_number(json_token_list *tokens, size_t *position,
								JSONValue *out_value) {
	JSONToken token;
	Result r = parser_expect_token(tokens, position, JSONTok_Number, &token);
	if (!r.success) {
		return r;
	}
	out_value->type = JSON_NUMBER;
	out_value->number = token.number;
	return new_success();
}

static Result parse_json_value(json_token_list *tokens, size_t *position,
							   JSONValue *out_value) {
	JSONToken token;
	check(parser_peek_token(tokens, *position, &token));
	switch (token.type) {
	case JSONTok_Number:
		return parse_json_number(tokens, position, out_value);
	case JSONTok_String:
		out_value->type = JSON_STRING;
		string_new(&out_value->str, "");
		string_append(&out_value->str, &token.value);
		check(parser_consume_token(tokens, position, &token));
		return new_success();
	case JSONTok_True:
		out_value->type = JSON_BOOL;
		out_value->boolean = true;
		check(parser_consume_token(tokens, position, &token));
		return new_success();
	case JSONTok_False:
		out_value->type = JSON_BOOL;
		out_value->boolean = false;
		check(parser_consume_token(tokens, position, &token));
		return new_success();
	case JSONTok_Null:
		out_value->type = JSON_NULL;
		check(parser_consume_token(tokens, position, &token));
		return new_success();
	case JSONTok_LBracket:
		return parse_json_array(tokens, position, out_value);
	case JSONTok_LBrace:
		return parse_json_object(tokens, position, out_value);
	default:
		return new_errorf(
			"Unexpected token %s at line %zu, column %zu (expected value)",
			EParserSyntaxError, tk_as_str(token.type), token.line,
			token.column);
	}
}

static Result parse_json_value_top_level(json_token_list *tokens,
										 size_t *position,
										 JSONValue *out_value) {
	Result r = parse_json_value(tokens, position, out_value);
	if (!r.success) {
		return r;
	}
	// Expect EOF
	JSONToken token;
	r = parser_expect_token(tokens, position, JSONTok_EOF, &token);
	if (!r.success) {
		json_value_free(out_value);
		return r;
	}
	return new_success();
}

Result deserialize_json(string *json, JSONValue *result) {
	Lexer lexer;
	lexer_init(&lexer, json);
	json_token_list tokens;
	json_token_list_init(&tokens, 0);
	Result r = get_tokens(&lexer, &tokens);
	if (!r.success) {
		return r;
	}
	size_t position = 0;
	r = parse_json_value_top_level(&tokens, &position, result);
	// Free tokens
	JSONToken token;
	for (size_t i = 0; i < tokens.length; i++) {
		json_token_list_get(&tokens, i, &token);
		lexer_free_token(&token);
	}
	json_token_list_free(&tokens);
	return r;
}

#define TYPE JSONToken
#define TYPED_NAME(name) json_token_##name
#define LIST_IMPLEMENTATION
#include "../../utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
