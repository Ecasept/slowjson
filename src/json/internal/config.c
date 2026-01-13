#include "../config.h"

ParserConfig config_default_parser_config(void) {
	ParserConfig config;
	config.double_overflow_behavior = CONFIG_DOUBLE_OVERFLOW_INF;
	config.exponent_overflow_behavior = CONFIG_EXPONENT_OVERFLOW_DOUBLE_FALLBACK;
	config.integer_overflow_behavior = CONFIG_INTEGER_OVERFLOW_DOUBLE_FALLBACK;
	config.limits = (ParserLimits){
		.max_nesting_depth = 1000,
	};
	return config;
}
