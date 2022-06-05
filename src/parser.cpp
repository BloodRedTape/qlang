#include "parser.hpp"
#include "ast/function.hpp"
#include "ast/statement.hpp"


Ast Parser::Parse(TokenStream tokens){
	Ast ast;

	for(int i = 0; !tokens.End(); i++){
		if (tokens.Peek().IsType(TokenType::Semicolon)) {
			tokens.Consume();
			continue;
		}
		
		AstFunctionDeclaration func;
		u64 count = func.TryParse(tokens, 0);
		if (count) {
			tokens.Consume(count);
			ast.push_back(AstNew<AstFunctionDeclaration>(std::move(func)));
			continue;
		}
		
		Error("Parser", "Can't parse top-level AST Node, Unexpected token '%'", TokenTypeString(tokens.Peek().Type));
		break;
	}

	return ast;
}
