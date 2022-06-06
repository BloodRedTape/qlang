#include "expression.hpp"
#include "error.hpp"
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

u64 TryParseBinaryOperator(BinaryOperatorType& type, const TokenStream &stream, u64 start){
	if (stream.Peek(start).IsType(TokenType::Equal)) {
		if(stream.Peek(start + 1).IsType(TokenType::Equal))
			return (type = BinaryOperatorType::IsEqual, 2);
		else
			return (type = BinaryOperatorType::Assign,  1);
	}

	if(stream.Peek(start).IsType(TokenType::Mult))
		return (type = BinaryOperatorType::Mult, 1);

	return 0;
}

u64 ParseSubexpression(ExpressionRef& expr, const TokenStream& stream, u64 start, u64 length) {
	if (stream.Peek(start + 0).IsType(TokenType::OpenParentheses)) {
		u64 stack = 0;
		for (u64 count = 0; count < length; count++) {
			u64 offset = start + count;

			if (stream.Peek(offset).IsType(TokenType::OpenParentheses))
				stack++;
			if (stream.Peek(offset).IsType(TokenType::CloseParentheses))
				stack--;

			if (!stack) {
				u64 sub_count = Expression::TryParse(expr, stream, start + 1, count - 1);
				return sub_count ? sub_count + 2 : 0;
			}
		}
		assert(expr);
	}

	// Non-Chainable expressions parsing
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
		u64 count = Expression::TryParse(sub_expr, stream, start + 1, length - 1);
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
	if (length == 1 && stream.Peek(start).IsType(TokenType::Identifier)) {
		expr = ExprNew<VariableExpression>(stream.Peek(start).IdentifierIndex);
		return 1;
	}

	return 0;
}

u64 Expression::TryParse(ExpressionRef& expr, const TokenStream& stream, u64 start, u64 length){
	for (u64 i = 0; i < length; i++) {
		Print("%", stream.Peek(start + i));
	}
	Print("\n");
	


	// Chainable expressions parsing
	{
	std::vector<BinaryOperatorType> ops;
	std::vector<ExpressionRef> exprs;
	u64 offset = 0;
	for (;;) {
		ExpressionRef expr;
		u64 expr_count = ParseSubexpression(expr, stream, start + offset, length - offset);
		if(!expr_count){
			if(offset == length) 
				break;
			else
				Error("Expression", "Can't parse subexpression");
		} else {
			exprs.push_back(std::move(expr));
			offset += expr_count;
		}
		
		BinaryOperatorType op = BinaryOperatorType::None;
		u64 op_count = TryParseBinaryOperator(op, stream, start + offset);

		if (op_count) {
			ops.push_back(op);
			offset += op_count;
		}
	}
	if(exprs.size() == 1 && ops.size() == 0)
		return (expr = std::move(exprs[0]), offset);

	if(offset)
		return offset;
	}

	return Error("Expression", "Unknown expression type starts with '%' of length %", stream.Peek(start), length);
}

