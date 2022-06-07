#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "print.hpp"
#include "ast/function.hpp"
#include "ast/statement.hpp"
#include "ast/ast.hpp"
#include "ast_printer.hpp"
#include "semantic.hpp"
#include <fstream>

std::string ReadEntireFile(const char* filepath) {
    std::fstream file(filepath);
    if(!file.is_open())
        return {};
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string content(size + 1, '\0');
    file.read(content.data(), size);
    return content;
}

void PrintTokenStream(const std::vector<Token>& tokens, const SymbolTable& table) {
    for (const Token &token : tokens){
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

    std::string src = ReadEntireFile("../../../examples/src.q");

    std::vector<Token> tokens = Lexer::DoLexicalAnalysis(src, table);

    PrintTokenStream(tokens, table);

    std::vector<AstNodeRef> ast = Parser::Parse(tokens);


    AstPrinter().Print(std::cout, ast, table);
    return !SemanticAnalyzer(ast, table).Analyze();
}