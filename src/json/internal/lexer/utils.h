#pragma once
#include "../lexer.h"

Result lexer_peek(const Lexer *lexer, UCP *out);
Result lexer_consume(Lexer *lexer, UCP *out);
