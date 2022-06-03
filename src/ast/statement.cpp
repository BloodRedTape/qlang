#include "ast/statement.hpp"

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
	default:
		assert(false);
	}
	
	return 0;
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
		return 0;

	for (;;) {
		if(stream.Peek(start + offset).IsType(TokenType::CloseBraces))
			return offset + 1;

		std::unique_ptr<AstStatement> stmt;
		u64 count = AstStatement::TryParse(stmt, stream, start + offset);
		if (!count)
			break;

		Statements.push_back(std::move(stmt));
		offset += count;
	}

	return 0;
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
	
	return 0;
}