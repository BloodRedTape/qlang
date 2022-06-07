#pragma once

#include "ast/ast.hpp"
#include "ast/function.hpp"

class SemanticAnalyzer {
public:
	static bool Analyze(const Ast &ast, const SymbolTable &table);
private:
	static bool CheckFunction(const AstFunctionDeclaration &func, const SymbolTable &table);
};