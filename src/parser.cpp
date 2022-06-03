#include "parser.hpp"
#include "ast/function.hpp"
#include "ast/statement.hpp"


std::vector<AstNodeRef> Parser::Parse(TokenStream tokens){
	Ast ast;

	for(int i = 0; !tokens.End(); i++){
		if (tokens.Peek().IsType(TokenType::Semicolon)) {
			tokens.Consume();
			continue;
		}
		
		{
		AstFunctionDeclaration func;
		u64 count = func.TryParse(tokens, 0);
		if (count) {
			tokens.Consume(count);
			ast.push_back(AstNew<AstFunctionDeclaration>(std::move(func)));
			continue;
		}

		}
		
		{
		AstStatementRef stmt;
		u64 count = AstStatement::TryParse(stmt, tokens, 0);

		if (count) {
			AstStatement *stmt_ptr = stmt.release();
			tokens.Consume(count);
			ast.push_back(std::unique_ptr<AstNode>(stmt_ptr));
			continue;
		}

		}

		assert(false);
	}

	return ast;
}
