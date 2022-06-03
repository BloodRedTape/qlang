#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "print.hpp"

const char *src = R"(

fn Add(int a, int b): int{
    return a + b;
}

extern fn Println(int number): void;

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

void PrintAst(const std::vector<AstNodeRef> &ast, const SymbolTable& table) {
    for (const AstNodeRef& node : ast) {
        switch (node->Type) {
        case AstNodeType::FunctionDeclaration: {
            AstFunctionDeclaration *decl = (AstFunctionDeclaration *)node.get();
            Print("%fn %",
                (decl->Qualifiers == FunctionQualifier::Extern) ? "extern " : "",
                table[decl->Identifier]
            );
            Print("(");
            for(int i = 0; decl->Parameters.size(); i++){
                AstVariableDefinition param = decl->Parameters[i];
                Print("% %", KeywordTypeString(param.DataType), table[param.IdentifierIndex]);
                if(i == decl->Parameters.size() - 1)
                    break;
                Print(", ");
            }
            Println("):%", KeywordTypeString(decl->ReturnType));

        }break;
        case AstNodeType::FunctionDefinition:
            break;
        case AstNodeType::VariableDefinition:
            break;
        default:
            assert(false);
        }
    }
}

int main() {
    SymbolTable table;

    std::vector<Token> tokens = Lexer::DoLexicalAnalysis(src, table);

    PrintTokenStream(tokens, table);

    std::vector<AstNodeRef> ast = Parser::Parse(tokens);
    
    PrintAst(ast, table);
}