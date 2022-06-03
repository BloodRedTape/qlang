#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "print.hpp"
#include "ast/function.hpp"
#include "ast/statement.hpp"
#include "ast/ast.hpp"

const char *src = R"(


fn Main(): int{
	int n;
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

void PrintStatement(const AstStatement* stmt, const SymbolTable &table) {
    switch (stmt->StmtType) {
    case StatementType::Empty:
        Print(";");
        break;
    case StatementType::Compound:{
        Print("{");
        CompoundStatement *compound = (CompoundStatement*)stmt;
        for(const AstStatementRef &stmt: compound->Statements)
            PrintStatement(stmt.get(), table);
        Print("}");
    }break;
    case StatementType::Var: {
        VarStatement *var = (VarStatement*)stmt;
        
        Print("% %", KeywordTypeString(var->DataType), table[var->IdentifierIndex]);
        assert(!var->InitialValue);
    }break;
    default:
        assert(false);
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
                FunctionParamDecl param = decl->Parameters[i];
                Print("% %", KeywordTypeString(param.DataType), table[param.IdentifierIndex]);
                if(i == decl->Parameters.size() - 1)
                    break;
                Print(", ");
            }
            Println("):%", KeywordTypeString(decl->ReturnType));

            if (decl->Body)
                PrintStatement(&*decl->Body, table);

        }break;
        case AstNodeType::Statement: {
            AstStatement *stmt = (AstStatement*)node.get();
            PrintStatement(stmt, table);
        }break;
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