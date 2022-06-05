#include "expression.hpp"
#include <assert.h>

const char* UnaryOperatorTypeString(UnaryOperatorType type){
	switch(type){
	case UnaryOperatorType::Negative: return "Negative";
	case UnaryOperatorType::PreIncrement: return "PreIncrement";
	case UnaryOperatorType::PreDecrement: return "PreDecrement";
	case UnaryOperatorType::PostIncrement: return "PostIncrement";
	case UnaryOperatorType::PostDecrement: return "PostDecrement";
	default: return (assert(false), "Shit happens");
	}
}

u64 Expression::TryParse(ExpressionRef& expr, const TokenStream& stream, u64 start){
	
	
	if(stream.HasRepeated(TokenType::Minus, 2, start)
	&& stream.Peek(start + 2).IsType(TokenType::Identifier)) {
		expr = ExprNew<UnaryOperatorExpression>(
			UnaryOperatorType::PreDecrement,
			ExprNew<VariableExpression>(stream.Peek(start + 2).IdentifierIndex)
		);
		return 3;
	}
	if(stream.HasRepeated(TokenType::Plus, 2, start)
	&& stream.Peek(start + 2).IsType(TokenType::Identifier)) {
		expr = ExprNew<UnaryOperatorExpression>(
			UnaryOperatorType::PreIncrement,
			ExprNew<VariableExpression>(stream.Peek(start + 2).IdentifierIndex)
		);
		return 3;
	}
	if(stream.Peek(start).IsType(TokenType::Identifier)
	&& stream.HasRepeated(TokenType::Minus, 2, start + 1)){
		expr = ExprNew<UnaryOperatorExpression>(
			UnaryOperatorType::PostDecrement,
			ExprNew<VariableExpression>(stream.Peek(start).IdentifierIndex)
		);
		return 3;
	}
	if(stream.Peek(start).IsType(TokenType::Identifier)
	&& stream.HasRepeated(TokenType::Plus, 2, start + 1)){
		expr = ExprNew<UnaryOperatorExpression>(
			UnaryOperatorType::PostIncrement,
			ExprNew<VariableExpression>(stream.Peek(start).IdentifierIndex)
		);
		return 3;
	}
	
	// parse only after predecrement, not to be folled
	if(stream.Peek(start).IsType(TokenType::Minus)){
		ExpressionRef sub_expr;
		u64 count = Expression::TryParse(sub_expr, stream, start + 1);
		if(!count)
			return 0;
		expr = ExprNew<UnaryOperatorExpression>(UnaryOperatorType::Negative, std::move(sub_expr));
		return count + 1;
	}

	if (stream.Peek(start).IsType(TokenType::IntegerLiteral)) {
		expr = ExprNew<IntegerLiteralExpression>(stream.Peek(start).IntegerLiteralValue);
		return 1;
	}
	
	// this check should be the last one, 
	// after we catched operator[], operator() operator++ operator--
	if (stream.Peek(start + 0).IsType(TokenType::Identifier)
	&& !stream.Peek(start + 1).IsType(TokenType::OpenBrackets)
	&& !stream.Peek(start + 1).IsType(TokenType::OpenParentheses)) {
		expr = ExprNew<VariableExpression>(stream.Peek(start).IdentifierIndex);
		return 1;
	}


	return 0;
}

