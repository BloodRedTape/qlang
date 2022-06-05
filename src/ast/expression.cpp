#include "expression.hpp"

u64 Expression::TryParse(ExpressionRef& expr, const TokenStream& stream, u64 start){
	if (stream.Peek(start).IsType(TokenType::IntegerLiteral)) {
		expr = ExprNew<IntegerLiteralExpression>(stream.Peek(start).IntegerLiteralValue);
		return 1;
	}
	if (stream.Peek(start + 0).IsType(TokenType::Identifier)
	&& !stream.Peek(start + 1).IsType(TokenType::OpenBrackets)
	&& !stream.Peek(start + 1).IsType(TokenType::OpenParentheses)) {
		expr = ExprNew<VariableExpression>(stream.Peek(start).IdentifierIndex);
		return 1;
	}
	}
	return 0;
}

