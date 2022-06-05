#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "print.hpp"
#include "ast/function.hpp"
#include "ast/statement.hpp"
#include "ast/ast.hpp"
#include "ast_printer.hpp"

const char *src = R"(

extern fn Print(int num): void;

fn SomeWierdFunc(int a, int b): int{

}

fn Main(int argc): int{
	int none;
	int n = 3;
    int neg = -n;
    int pi = n++;
    int pd = n--;
    int pri = ++n;
    int prd = --n;

    {
        int d = 5;
        int c = d;
        {}
    }

}

)";

void PrintTokenStream(const std::vector<Token>& tokens, const SymbolTable& table) {
    for (Token token : tokens){
        if(token.Type == TokenType::Identifier)
            Println("Token: % => %", TokenTypeString(token.Type), table[token.IdentifierIndex]);
        else if(token.Type == TokenType::IntegerLiteral)
            Println("Token: % = '%'", TokenTypeString(token.Type), token.IntegerLiteralValue);
        else
            Println("Token: %", TokenTypeString(token.Type));
    }
}

int main() {
    SymbolTable table;

    std::vector<Token> tokens = Lexer::DoLexicalAnalysis(src, table);

    PrintTokenStream(tokens, table);

    std::vector<AstNodeRef> ast = Parser::Parse(tokens);
    
    //PrintAst(ast, table);

    AstPrinter().Print(std::cout, ast, table);
}