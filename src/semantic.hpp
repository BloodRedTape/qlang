#pragma once

#include <set>
#include <vector>
#include "ast/ast.hpp"
#include "ast/function.hpp"



class Scope {
private:
	std::set<TokenAtom> m_Identifiers;
public:
	bool Has(TokenAtom identifier)const {
		return m_Identifiers.find(identifier) != m_Identifiers.end();
	}

	void Declare(TokenAtom identifier) {
		m_Identifiers.insert(identifier);
	}
};

class SemanticAnalyzer {
private:
	const Ast &m_Ast;
	const SymbolTable &m_Table;
	Stack<Scope> m_Stack;
public:
	SemanticAnalyzer(const Ast &ast, const SymbolTable &table);

	bool Analyze();
private:
	bool CheckFunction(const AstFunctionDeclaration &func);

	bool CheckExpression(const Expression &expr);

	bool CheckStatement(const AstStatement &stmt);

	bool Find(TokenAtom identifier)const{
		for (const Scope& scope : m_Stack) {
			if(scope.Has(identifier))
				return true;
		}
		return false;
	}

	bool IsFunctionDefined(TokenAtom identifier);
};