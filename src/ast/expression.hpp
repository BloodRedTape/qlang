#pragma once

#include <memory>

enum class ExpressionType {
	None = 0,
	Literal,
	Variable,
	UnaryOperator,
	BinaryOperator,
	Call
};

struct Expression {
	ExpressionType Type = ExpressionType::None;
	//some type identifier for expression result type
	//for now all expressions results in int
};

using ExpressionRef = std::unique_ptr<struct Expression>;

struct LiteralExpression : Expression {
	u64 Value = 0;

	LiteralExpression() {Type = ExpressionType::Literal; }
};

struct VariableExpression : Expression {
	TokenAtom IdentifierIndex = InvalidTokenAtom;

	VariableExpression() {Type = ExpressionType::Variable; }
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
