#pragma once

#include <ostream>
#include "ast/ast.hpp"
#include "ast/statement.hpp"

class AstPrinter {
private:
	u64 m_Scope = 0;
public:
	void Print(std::ostream &stream, const Ast &ast, const SymbolTable &table);
private:
	void Indent(){m_Scope++;}

	void UnIndent(){m_Scope--;}

	void PrintIndent(std::ostream &stream);
private:
	void PrintAstNode(std::ostream &stream, const AstNodeRef& ast, const SymbolTable& table);
	
	void PrintStatement(std::ostream &stream, const AstStatement* stmt, const SymbolTable &table);

	void PrintExpression(std::ostream &stream, const ExpressionRef &expr, const SymbolTable &table);
};