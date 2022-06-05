#include "expression.hpp"

u64 Expression::TryParse(ExpressionRef& expr, const TokenStream& stream, u64 start){
	if (stream.Peek(start).IsType(TokenType::IntegerLiteral)) {
		expr = ExprNew<IntegerLiteralExpression>(stream.Peek(start).IntegerLiteralValue);
		return 1;
	}
	return 0;
}

