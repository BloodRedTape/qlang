#include "ast/statement.hpp"
#include "error.hpp"

const char* StatementTypeString(StatementType type){
	switch (type) {
	case StatementType::Empty: return "Empty";
	case StatementType::Compound: return "Compound";
	case StatementType::If: return "If";
	case StatementType::Var: return "Var";
	case StatementType::Expression: return "Expression";
	case StatementType::For: return "For";
	case StatementType::Return: return "Return";
	default: return (assert(false), "Shit happens");
	}
}

u64 AstStatement::TryParse(AstStatementRef& out_stmt, const TokenStream& stream, size_t start){
	StatementType type = DeclStatementType(stream, start);
	
	switch (type) {
	case StatementType::Empty:
		assert(stream.Peek(start).IsType(TokenType::Semicolon));

		out_stmt = StmtNew<AstStatement>();
		return 1;
	case StatementType::Compound: {
		CompoundStatement stmt;
		u64 count = stmt.TryParse(stream, start);
		if(count)
			return (out_stmt = StmtNew<CompoundStatement>(std::move(stmt)), count);
	}break;
	case StatementType::Var: {
		VarStatement stmt;
		u64 count = stmt.TryParse(stream, start);
		if(count)
			return (out_stmt = StmtNew<VarStatement>(std::move(stmt)), count);
	}break;
	case StatementType::Expression: {
		ExpressionStatement expr;
		u64 count = expr.TryParse(stream, start);
		if(count)
			return (out_stmt = StmtNew<ExpressionStatement>(std::move(expr)), count);
	}break;
	default:
		assert(false);
	}
	
	return Error("Statement", "Can't parse '%Statement' starts with token '%'", StatementTypeString(type), stream.Peek());
}

StatementType DeclStatementType(const TokenStream& stream, size_t start){
	Token first = stream.Peek(start);

	if(first.IsType(TokenType::Semicolon))
		return StatementType::Empty;

	if(first.IsType(TokenType::OpenBraces))
		return StatementType::Compound;

	if(first.IsKeyword(KeywordType::If))
		return StatementType::If;

	if(first.IsDataType())
		return StatementType::Var;
	
	if(first.IsKeyword(KeywordType::For))
		return StatementType::For;

	if(first.IsKeyword(KeywordType::Return))
		return StatementType::Return;

	return StatementType::Expression;
}

u64 CompoundStatement::TryParse(const TokenStream& stream, size_t start){
	u64 offset = 0;
	if(!stream.Peek(start + offset++).IsType(TokenType::OpenBraces))
		return Error("CompoundStatement", "Compound statement should start with OpenBraces");

	for (;;) {
		if(stream.Peek(start + offset).IsType(TokenType::CloseBraces))
			return offset + 1;

		std::unique_ptr<AstStatement> stmt;
		u64 count = AstStatement::TryParse(stmt, stream, start + offset);
		if (!count)
			break;

		offset += count;

		if(stmt->StmtType == StatementType::Empty)
			continue;

		Statements.push_back(std::move(stmt));
	}

	return Error("CompoundStatement", "Can't parse iternal statement");
}

u64 VarStatement::TryParse(const TokenStream& stream, u64 start){
	Token type		 = stream.Peek(start + 0);
	Token identifier = stream.Peek(start + 1);

	if(!type.IsKeyword(KeywordType::Int) || !identifier.IsType(TokenType::Identifier))
		return 0;

	DataType = type.KeywordIndex;
	IdentifierIndex = identifier.IdentifierIndex;

	if(stream.Peek(start + 2).IsType(TokenType::Semicolon))
		return 2;

	if(!stream.Peek(start + 2).IsType(TokenType::Equal))
		return 0;
	
	u64 count = ExpressionStatement::TryParse(InitialValue, stream, start + 3);

	if(!count)
		return 0;

	return count + 3;
}

u64 ExpressionStatement::TryParse(const TokenStream& stream, u64 start){
	return TryParse(Expr, stream, start);
}

u64 ExpressionStatement::TryParse(ExpressionRef& expr, const TokenStream& stream, u64 start){
	u64 count = stream.CountUntilFirst(TokenType::Semicolon, start);

	if(count == -1)
		return 0;

	return Expression::TryParse(expr, stream, start, count);
}
