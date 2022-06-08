#pragma once

#include <sstream>
#include "ast/ast.hpp"
#include "ast/function.hpp"
#include "ast/statement.hpp"
#include "ast/expression.hpp"

class CCodegen {
private:
	const Ast &m_Ast;
	const SymbolTable &m_Table;
	std::stringstream m_SourceCode;
public:
	CCodegen(const Ast &ast, const SymbolTable &table);

	std::string Generate();
private:
	std::string HeaderBoilderplate()const;

	std::string GenerateFunctionHeader(const AstFunctionDeclaration &decl);

	std::string GenerateFunctionDeclaration(const AstFunctionDeclaration &decl);

	std::string GenerateFunctionDefinition(const AstFunctionDeclaration &decl);

	std::string GenerateStatement(const AstStatement &stmt);

	std::string GenerateExpression(const Expression &expr);

	std::string GenerateEntryPoint()const;
};


class CCompiler {
public:
	static bool Compile(const std::string &src);
};
