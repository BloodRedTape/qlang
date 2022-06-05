#pragma once

#include "lexer.hpp"
#include "error.hpp"
#include "ast/ast.hpp"
#include <memory>
#include <vector>
#include <ostream>

class Parser {
public:
	static Ast Parse(TokenStream tokens);
};
