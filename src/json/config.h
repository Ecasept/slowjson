#pragma once

enum ConfigExponentOutOfRangeBehavior {
	// Throws an error when an integer is out of range
	CONFIG_EXPONENT_OVERFLOW_ERROR,
	// Clamps the integer to its maximum/minimum value when out of range
	CONFIG_EXPONENT_OVERFLOW_CLAMP,
	// Falls back to double representation when exponent is out of range
	CONFIG_EXPONENT_OVERFLOW_DOUBLE_FALLBACK
};
typedef enum ConfigExponentOutOfRangeBehavior ConfigExponentOutOfRangeBehavior;

enum ConfigIntegerOutOfRangeBehavior {
	// Throws an error when an integer is out of range
	CONFIG_INTEGER_OVERFLOW_ERROR,
	// Clamps the integer to its maximum/minimum value when out of range
	CONFIG_INTEGER_OVERFLOW_CLAMP,
	// Falls back to double representation when integer is out of range
	CONFIG_INTEGER_OVERFLOW_DOUBLE_FALLBACK,
};
typedef enum ConfigIntegerOutOfRangeBehavior ConfigIntegerOutOfRangeBehavior;

enum ConfigDoubleOutOfRangeBehavior {
	// Throws an error when a double is out of range
	CONFIG_DOUBLE_OVERFLOW_ERROR,
	// Clamps the double to its maximum/minimum value when out of range
	CONFIG_DOUBLE_OVERFLOW_CLAMP,
	/** 
	 * @brief Use positive/negative infinity when out of range
	 * @warning Using this will make you unable to serialize the deserialized JSON back to JSON,
	 * since JSON does not support infinity values.
	 */
	CONFIG_DOUBLE_OVERFLOW_INF,
};
typedef enum ConfigDoubleOutOfRangeBehavior ConfigDoubleOutOfRangeBehavior;

/**
 * @brief Configuration options for the JSON parser
 * @note A note on number parsing:
 * If a serialized double can not be deserialized to the nearest representable double value,
 * using Clinger's fast path, the parser will fall back to a naive method that may not return
 * the nearest representable value.
 */
struct ParserConfig {
	/**
	 * @brief If the exponent is larger than the largest integer type supported (intmax_t),
	 * the behavior specified here is applied.
	 */
	ConfigExponentOutOfRangeBehavior exponent_overflow_behavior;
	/**
	 * @brief If a number can be decoded as an integer but is larger than what can be represented
	 * by intmax_t, the behavior specified here is applied.
	 */
	ConfigIntegerOutOfRangeBehavior integer_overflow_behavior;
	/**
	 * @brief If a number is decoded as a double but is outside the representable
	* range of a double, the behavior specified here is applied.
	*/
	ConfigDoubleOutOfRangeBehavior double_overflow_behavior;
};
typedef struct ParserConfig ParserConfig;

ParserConfig config_default_parser_config(void);
