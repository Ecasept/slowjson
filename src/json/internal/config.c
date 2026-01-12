#include "../config.h"

ParserConfig config_default_parser_config(void) {
	ParserConfig config;
	config.double_overflow_behavior = CONFIG_DOUBLE_OVERFLOW_ERROR;
	config.exponent_overflow_behavior = CONFIG_EXPONENT_OVERFLOW_ERROR;
	config.integer_overflow_behavior = CONFIG_INTEGER_OVERFLOW_ERROR;
	return config;
}
