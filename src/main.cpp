#include <iostream>
#include "lexer.hpp"
#include "print.hpp"

const char *src = R"(

extern fn Println(int number);

fn Factorial(int num): int{
    if(num == 0)
        return 1;
    else
        return Factorial(num - 1) * num;
}

fn Main(): int{
    int[5] table;

    for(int i = 0; i<5; i++)
        table[i] = Factorial(i);

    for(int i = 0; i<5; i++)
        Println(table[i]);

    return 0;
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
}