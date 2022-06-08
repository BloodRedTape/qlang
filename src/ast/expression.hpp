#pragma once

#include <memory>
#include "ast.hpp"

enum class ExpressionType {
	None = 0,
	IntegerLiteral,
	BoolLiteral,
	StringLiteral,
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

	static u64 TryParse(ExpressionRef &expr, const TokenStream &stream, u64 start, u64 length);
};

struct IntegerLiteralExpression : Expression {
	u64 Value = 0;

	IntegerLiteralExpression(u64 value) {
		Type = ExpressionType::IntegerLiteral; 
		ResultType = KeywordType::Int;
		Value = value;
	}
};

struct BoolLiteralExpression : Expression {
	u64 Value = 0;

	BoolLiteralExpression(u64 value) {
		Type = ExpressionType::BoolLiteral; 
		ResultType = KeywordType::Bool;
		Value = value;
	}
};

struct StringLiteralExpression : Expression {
	TokenAtom Value = 0;

	StringLiteralExpression(TokenAtom value) {
		Type = ExpressionType::StringLiteral; 
		ResultType = KeywordType::String;
		Value = value;
	}
};

struct VariableExpression : Expression {
	Token Identifier;

	VariableExpression(Token identifier):
		Identifier(identifier)
	{
		Type = ExpressionType::Variable; 
	}
};

enum class UnaryOperatorType {
	Negative,
	PreIncrement,
	PreDecrement,
	PostIncrement,
	PostDecrement
};

const char *UnaryOperatorTypeString(UnaryOperatorType type);

struct UnaryOperatorExpression : Expression {
	ExpressionRef Expr;
	UnaryOperatorType UnaryType;

	UnaryOperatorExpression(UnaryOperatorType unary_type, ExpressionRef expr):
		UnaryType(unary_type),
		Expr(std::move(expr))
	{ Type = ExpressionType::UnaryOperator; }
};

enum class BinaryOperatorType {
	None = 0,
	Assign,	
	//ArraySubscript,
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

const char *BinaryOperatorTypeString(BinaryOperatorType op);

const char *BinaryOperatorTokenString(BinaryOperatorType op);

u64 BinaryOperatorPriority(BinaryOperatorType type);

u64 TryParseBinaryOperator(BinaryOperatorType &type, const TokenStream &stream, u64 start);

struct BinaryOperatorExpression : Expression {
	BinaryOperatorType OpType = BinaryOperatorType::None;
	ExpressionRef Left;
	ExpressionRef Right;

	BinaryOperatorExpression(){ Type = ExpressionType::BinaryOperator; }

	BinaryOperatorExpression(ExpressionRef left, ExpressionRef right, BinaryOperatorType op) {
		Type = ExpressionType::BinaryOperator;
		Left = std::move(left);
		Right = std::move(right);
		OpType = op;
	}
};

struct CallExpression : Expression {
	Token Identifier;
	std::vector<ExpressionRef> Args;

	CallExpression(Token identifier):
		Identifier(identifier)
	{ Type = ExpressionType::Call; }
};
