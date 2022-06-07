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

const char* BinaryOperatorTypeString(BinaryOperatorType op){
	switch(op){
	case BinaryOperatorType::Assign: return "Assign";
	case BinaryOperatorType::Add: return "Add";
	case BinaryOperatorType::Sub: return "Sub";
	case BinaryOperatorType::Mult: return "Mult";
	case BinaryOperatorType::Div: return "Div";
	case BinaryOperatorType::IsEqual: return "IsEqual";
	case BinaryOperatorType::IsNotEqual: return "IsNotEqual";
	case BinaryOperatorType::Less: return "Less";
	case BinaryOperatorType::LessEqual: return "LessEqual";
	case BinaryOperatorType::More: return "More";
	case BinaryOperatorType::MoreEqual: return "MoreEqual";
	default: return (assert(false), "Shit happens");
	}
}

u64 BinaryOperatorPriority(BinaryOperatorType type){
	switch (type) {
	case BinaryOperatorType::Assign: return 1;
	case BinaryOperatorType::Add: return 3;
	case BinaryOperatorType::Sub: return 3;
	case BinaryOperatorType::Mult: return 4;
	case BinaryOperatorType::Div: return 4;
	case BinaryOperatorType::IsEqual: return 2;
	case BinaryOperatorType::IsNotEqual: return 2;
	case BinaryOperatorType::Less: return 2;
	case BinaryOperatorType::LessEqual: return 2;
	case BinaryOperatorType::More: return 2;
	case BinaryOperatorType::MoreEqual: return 2;
	default: return (assert(false), 0);
	}
}

u64 TryParseBinaryOperator(BinaryOperatorType& type, const TokenStream &stream, u64 start){
	if (stream.Peek(start).IsType(TokenType::Equal)) {
		if(stream.Peek(start + 1).IsType(TokenType::Equal))
			return (type = BinaryOperatorType::IsEqual, 2);
		else
			return (type = BinaryOperatorType::Assign,  1);
	}

	if(stream.Peek(start).IsType(TokenType::Plus))
		return (type = BinaryOperatorType::Add, 1);
	if(stream.Peek(start).IsType(TokenType::Minus))
		return (type = BinaryOperatorType::Sub, 1);
	if(stream.Peek(start).IsType(TokenType::Mult))
		return (type = BinaryOperatorType::Mult, 1);
	if(stream.Peek(start).IsType(TokenType::Div))
		return (type = BinaryOperatorType::Div, 1);
	
	if(stream.Peek(start + 0).IsType(TokenType::Exclamation)
	&& stream.Peek(start + 1).IsType(TokenType::Equal))
		return (type = BinaryOperatorType::IsNotEqual, 2);

	if (stream.Peek(start).IsType(TokenType::Less)) {
		if(stream.Peek(start + 1).IsType(TokenType::Equal))
			return (type = BinaryOperatorType::LessEqual, 2);
		else 
			return (type = BinaryOperatorType::Less, 1);
	}

	if (stream.Peek(start).IsType(TokenType::More)) {
		if(stream.Peek(start + 1).IsType(TokenType::Equal))
			return (type = BinaryOperatorType::MoreEqual, 2);
		else 
			return (type = BinaryOperatorType::More, 1);
	}

	return 0;
}


u64 FindThisScopeComma(const TokenStream& stream, u64 start, u64 length) {
	u64 stack = 0;
	for (u64 i = 0; i < length; i++) {
		if(stream.Peek(start + i).IsType(TokenType::OpenParentheses)
		|| stream.Peek(start + i).IsType(TokenType::OpenBrackets)
		|| stream.Peek(start + i).IsType(TokenType::OpenBraces))
			stack++;
		if(stream.Peek(start + i).IsType(TokenType::CloseParentheses)
		|| stream.Peek(start + i).IsType(TokenType::CloseBrackets)
		|| stream.Peek(start + i).IsType(TokenType::CloseBraces))
			stack--;
		if(stream.Peek(start + i).IsType(TokenType::Comma))
			return i;
	}
	return InvalidIndex;
}

u64 ParseSubexpression(ExpressionRef& expr, const TokenStream& stream, u64 start, u64 length) {
	if(length == 0)
		return 0;
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
			ExprNew<VariableExpression>(stream.Peek(start + 2))
		);
		return 3;
	}
	if(stream.HasRepeated(TokenType::Plus, 2, start)
	&& stream.Peek(start + 2).IsType(TokenType::Identifier)) {
		expr = ExprNew<UnaryOperatorExpression>(
			UnaryOperatorType::PreIncrement,
			ExprNew<VariableExpression>(stream.Peek(start + 2))
		);
		return 3;
	}
	if(stream.Peek(start).IsType(TokenType::Identifier)
	&& stream.HasRepeated(TokenType::Minus, 2, start + 1)){
		expr = ExprNew<UnaryOperatorExpression>(
			UnaryOperatorType::PostDecrement,
			ExprNew<VariableExpression>(stream.Peek(start))
		);
		return 3;
	}
	if(stream.Peek(start).IsType(TokenType::Identifier)
	&& stream.HasRepeated(TokenType::Plus, 2, start + 1)){
		expr = ExprNew<UnaryOperatorExpression>(
			UnaryOperatorType::PostIncrement,
			ExprNew<VariableExpression>(stream.Peek(start))
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

	if (stream.Peek(start + 0).IsType(TokenType::Identifier)
	&&  stream.Peek(start + 1).IsType(TokenType::OpenParentheses)) {
		CallExpression call(stream.Peek(start + 0));
		
		u64 scope_length = stream.CountScopeSize(start + 1, TokenType::OpenParentheses, TokenType::CloseParentheses);
		assert(scope_length <= length - 1);

		if(stream.Peek(start + 2).IsType(TokenType::CloseParentheses))
			return (expr = ExprNew<CallExpression>(std::move(call)), 3);
		
		u64 offset = 2;
		for (;;) {
			ExpressionRef expr;
			u64 expr_len = FindThisScopeComma(stream, start + offset, scope_length - offset);
			u64 count = Expression::TryParse(expr, stream, start + offset, expr_len != InvalidIndex ? expr_len : scope_length - offset);

			if (count) {
				offset += count;
				
				TokenType end = stream.Peek(start + offset).Type;
				if(end == TokenType::Comma || end == TokenType::CloseParentheses){
					offset += 1;
					call.Args.push_back(std::move(expr));
				} else {
					Error("CallExpression", "Unexpected token '%'", stream.Peek(start + offset));
					break;
				}
				if(end == TokenType::CloseParentheses)
					break;
			}
		}
			
		return (expr = ExprNew<CallExpression>(std::move(call)), offset);
	}

	// this check should be the last one, 
	// after we catched operator[], operator() operator++ operator--
	if (stream.Peek(start + 0).IsType(TokenType::Identifier)
	&& !stream.Peek(start + 1).IsType(TokenType::OpenParentheses)
	&& !stream.Peek(start + 1).IsType(TokenType::OpenBrackets)) {
		expr = ExprNew<VariableExpression>(stream.Peek(start));
		return 1;
	}

	return 0;
}

struct ExpressionPrecedence{
	std::vector<ExpressionRef> Exprs;
	std::vector<BinaryOperatorType> Ops;

	ExpressionRef Solve() {
		assert(Exprs.size() == Ops.size() + 1);
		
		while (Ops.size()) {
			size_t highest = FindHighestPriorityOp();
			
			ExpressionRef expr = ExprNew<BinaryOperatorExpression>(
				std::move(Exprs[highest]),
				std::move(Exprs[highest + 1]),
				Ops[highest]
			);
			
			std::swap(Ops[highest], Ops.back());
			Ops.pop_back();
			std::swap(Exprs[highest + 1], Exprs.back());
			Exprs.pop_back();
			Exprs[highest] = std::move(expr);
		}

		return std::move(Exprs[0]);
	}

	size_t FindHighestPriorityOp() {
		size_t highest_index = InvalidIndex;
		u64 hightes_priority = 0;

		for (size_t i = 0; i<Ops.size(); i++) {
			u64 prior = BinaryOperatorPriority(Ops[i]);
			if (prior > hightes_priority) {
				highest_index = i;
				hightes_priority = prior;
			}
		}
		
		assert(highest_index != InvalidIndex);
		return highest_index;
	}

};

u64 Expression::TryParse(ExpressionRef& expr, const TokenStream& stream, u64 start, u64 length){	
	// Chainable expressions parsing
	
	ExpressionPrecedence precedence;

	u64 offset = 0;
	for (;;) {
		ExpressionRef expr;
		u64 expr_count = ParseSubexpression(expr, stream, start + offset, length - offset);
		if(!expr_count){
			if(offset == length) 
				break;
			else
				return Error("Expression", "Can't parse subexpression starts from token '%'", stream.Peek(start + offset));
		} else {
			precedence.Exprs.push_back(std::move(expr));
			offset += expr_count;
		}
		
		BinaryOperatorType op = BinaryOperatorType::None;
		u64 op_count = TryParseBinaryOperator(op, stream, start + offset);

		if (op_count) {
			precedence.Ops.push_back(op);
			offset += op_count;
		}
	}

	if(expr = precedence.Solve())
		return offset;

	return Error("Expression", "Unknown expression type starts with '%' of length %", stream.Peek(start), length);
}

