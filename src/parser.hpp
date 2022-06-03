#pragma once

#include "lexer.hpp"
#include "ast/ast.hpp"
#include <memory>
#include <vector>
#include <ostream>

class Parser {
public:
	static std::vector<AstNodeRef> Parse(TokenStream tokens);
};
