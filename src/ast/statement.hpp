#pragma once

#include <memory>
#include <vector>
#include "ast/ast.hpp"
#include "ast/expression.hpp"

enum class StatementType {
	Empty		= 0,
	Compound	= 1,
	If			= 2,
	Var			= 3,
	Expression	= 4,
	While		= 5,
	Return		= 6
};

const char *StatementTypeString(StatementType type);

using AstStatementRef = std::unique_ptr<struct AstStatement>;

struct AstStatement : AstNode {
	StatementType StmtType = StatementType::Empty;

	static u64 TryParse(AstStatementRef &stmt, const TokenStream &stream, size_t start);
};

StatementType DeclStatementType(const TokenStream &stream, size_t start);

template<typename Type, typename ...ArgsType>
AstStatementRef StmtNew(ArgsType&&...args) {
	return std::make_unique<Type>(std::forward<ArgsType>(args)...);
}


struct CompoundStatement: AstStatement{
	std::vector<AstStatementRef> Statements;

	CompoundStatement() {StmtType = StatementType::Compound; }

	u64 TryParse(const TokenStream &stream, size_t start);
};

struct VarStatement : AstStatement {
	KeywordType DataType = KeywordType::Count;
	TokenAtom IdentifierIndex = InvalidTokenAtom;
	ExpressionRef InitialValue;

	VarStatement() {StmtType = StatementType::Var; }

	u64 TryParse(const TokenStream &stream, u64 start);
};

struct ExpressionStatement : AstStatement {
	ExpressionRef Expr;

	ExpressionStatement() { StmtType = StatementType::Expression; }

	static u64 TryParse(ExpressionRef &expr, const TokenStream &stream, u64 start);

	u64 TryParse(const TokenStream &stream, u64 start);
};

struct WhileStatement : AstStatement {
	ExpressionRef Condition;
	CompoundStatement Body;

	WhileStatement() { StmtType = StatementType::While; }

	u64 TryParse(const TokenStream &stream, u64 start);
};

struct ReturnStatement : AstStatement {
	ExpressionRef Result;

	ReturnStatement() { StmtType = StatementType::Return; }

	u64 TryParse(const TokenStream &stream, u64 start);
};
