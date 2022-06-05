#pragma once

#include <memory>
#include "ast.hpp"

enum class ExpressionType {
	None = 0,
	IntegerLiteral,
	Variable,
	UnaryOperator,
	BinaryOperator,
	Call
};

using ExpressionRef = std::unique_ptr<struct Expression>;

template<typename Type, typename ...ArgsType>
ExpressionRef ExprNew(ArgsType&&...args) {
	return std::make_unique<Type>(std::forward<ArgsType>(args)...);
}

struct Expression {
	ExpressionType Type = ExpressionType::None;
	//some type identifier for expression result type
	//for now expr result type is represented as keyword
	KeywordType ResultType = KeywordType::Count;

	virtual ~Expression() = default;

	static u64 TryParse(ExpressionRef &expr, const TokenStream &stream, u64 start);
};

struct IntegerLiteralExpression : Expression {
	u64 Value = 0;

	IntegerLiteralExpression(u64 value) {
		Type = ExpressionType::IntegerLiteral; 
		ResultType = KeywordType::Int;
		Value = value;
	}
};

struct VariableExpression : Expression {
	TokenAtom IdentifierIndex = InvalidTokenAtom;

	VariableExpression(TokenAtom identifier) {
		Type = ExpressionType::Variable; 
		IdentifierIndex = identifier;
	}
};

enum class UnaryOperatorType {
	ArrayAccess,
	Increment,
	Decrement
};

struct UnaryOperatorExpression : Expression {
	ExpressionRef ref;

	UnaryOperatorExpression() { Type = ExpressionType::UnaryOperator; }
};

enum class BinaryOperatorType {
	None = 0,
	Assign,	
	Add,
	Sub,
	Mult,
	Div,
	IsEqual,
	IsNotEqual,
	Less,
	LessEqual,
	More,
	MoreEqual
};

struct BinaryOperatorExpression : Expression {
	BinaryOperatorType OpType = BinaryOperatorType::None;
	ExpressionRef Left;
	ExpressionRef Right;

	BinaryOperatorExpression(){ Type = ExpressionType::BinaryOperator; }
};

struct CallExpression : Expression {
	std::vector<ExpressionRef> Args;

	CallExpression() { Type = ExpressionType::Call; }
};
