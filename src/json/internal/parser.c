#include "parser.h"
#include "lexer.h"
#include "../deserialize.h"
#include <stdlib.h>
#include "../utils/alloc/default.h"

// Forward declaration
static Result parse_json_value(Parser *parser,
							   JSONValue *out_value, size_t depth);

static Result parser_peek_token(Parser *parser,
								JSONToken *out_token) {
	while (!parser->has_lookahead) {
		check(lexer_next_token(parser->lexer, &parser->lookahead));
		if (parser->lookahead.type == JSONTok_Whitespace) {
			lexer_free_token(parser->lexer, &parser->lookahead);
		} else {
			parser->has_lookahead = true;
		}
	}
	*out_token = parser->lookahead;
	return new_success();
}

/* Consuming a token transfers ownership of its string to the caller. */
static void parser_skip_token(Parser *parser) {
	parser->has_lookahead = false;
}
static Result parser_expect_token(Parser *parser,
								  JSONTokenType expected_type,
								  JSONToken *out_token) {
	JSONToken token;
	Result r = parser_peek_token(parser, &token);
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
	parser_skip_token(parser);
	return new_success();
}

static Result parse_json_array(Parser *parser, JSONValue *out_value, size_t depth) {
	Result r;
	JSONToken token;
	out_value->type = JSON_ARRAY;
	json_value_list_init(&out_value->list, 0, parser->document->value_allocator);

	r = parser_peek_token(parser, &token);
	if (!r.success)
		goto error;
	if (token.type == JSONTok_RBracket) {
		// Empty array
		parser_skip_token(parser);
		return new_success();
	}
	// Parse elements
	while (1) {
		JSONValue element;
		r = parse_json_value(parser, &element, depth + 1);
		if (!r.success)
			goto error;
		json_value_list_push(&out_value->list, element, parser->document->value_allocator);

		// Check for ',' or ']'
		r = parser_peek_token(parser, &token);
		if (!r.success)
			goto error;
		if (token.type == JSONTok_Comma) {
			parser_skip_token(parser);
		} else if (token.type == JSONTok_RBracket) {
			parser_skip_token(parser);
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
		json_value_free_split(&out_value->list.data[i], parser->document->value_allocator, parser->document->string_allocator);
	}
	json_value_list_free(&out_value->list, parser->document->value_allocator);
	return r;
}

static Result parse_json_object(Parser *parser, JSONValue *out_value, size_t depth) {
	JSONToken token;
	Result r;
	out_value->type = JSON_OBJECT;
	json_value_hashmap_init(&out_value->hashmap, parser->document->value_allocator);

	r = parser_peek_token(parser, &token);
	if (!r.success)
		goto error;
	if (token.type == JSONTok_RBrace) {
		// Empty object
		parser_skip_token(parser);
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
		if (!r.success) {
			lexer_free_token(parser->lexer, &key);
			goto error;
		}
		r = parse_json_value(parser, &value, depth + 1);
		if (!r.success) {
			lexer_free_token(parser->lexer, &key);
			goto error;
		}

		json_value_hashmap_set_split(&out_value->hashmap, key.value, value, parser->document->value_allocator, parser->document->string_allocator);

		// Check for ',' or '}'
		r = parser_peek_token(parser, &token);
		if (!r.success)
			goto error;
		if (token.type == JSONTok_Comma) {
			parser_skip_token(parser);
		} else if (token.type == JSONTok_RBrace) {
			parser_skip_token(parser);
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
	json_value_hashmap_free_split(&out_value->hashmap, parser->document->value_allocator, parser->document->string_allocator);
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
		// Copy the string value that was allocated by the lexer
		out_value->str = token.value;
		parser_skip_token(parser);
		return new_success();
	case JSONTok_True:
		out_value->type = JSON_BOOL;
		out_value->boolean = true;
		parser_skip_token(parser);
		return new_success();
	case JSONTok_False:
		out_value->type = JSON_BOOL;
		out_value->boolean = false;
		parser_skip_token(parser);
		return new_success();
	case JSONTok_Null:
		out_value->type = JSON_NULL;
		parser_skip_token(parser);
		return new_success();
	case JSONTok_LBracket:
		parser_skip_token(parser);
		return parse_json_array(parser, out_value, depth);
	case JSONTok_LBrace:
		parser_skip_token(parser);
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
		json_value_free_split(out_value, parser->document->value_allocator, parser->document->string_allocator);
		return r;
	}
	return new_success();
}

Parser json_parser_new(ParserConfig config) {
    Parser parser = {0};
    parser.config = config;
    return parser;
}

Result json_parser_deserialize(Parser *parser, string *json, JSONDocument *doc) {
    json_document_reset(doc);
    parser->document = doc;
    parser->has_lookahead = false;
    Lexer lexer;
    lexer_init(&lexer, json, parser->config, doc->string_allocator);
    parser->lexer = &lexer;

    Result r = parse_json_value_top_level(parser, &doc->value);
    if (parser->has_lookahead) {
        lexer_free_token(&lexer, &parser->lookahead);
    }
    parser->has_lookahead = false;
    parser->lookahead = (JSONToken){0};
    parser->lexer = NULL;
    parser->document = NULL;
    if (!r.success) {
        // Parsing routines already release any partially built containers.
        doc->value = json_value_new_null();
        json_document_reset(doc);
    }
    return r;
}
