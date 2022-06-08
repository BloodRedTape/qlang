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
#include "c_codegen.hpp"

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

std::string ToCCodegenFilepath(std::string filepath) {
    filepath[filepath.size() - 1] = 'c';
    return filepath + "pp";
}

std::string ToEXEFilepath(std::string filepath) {
    filepath[filepath.size() - 1] = 'e';
    return filepath + "xe";
}

int main(int argc, char **argv) {
    const char *src_filepath = (argc == 2 ? argv[1] : "../../../examples/src.q");
    SymbolTable table;

    std::string src = ReadEntireFile(src_filepath);

    std::vector<Token> tokens = Lexer::DoLexicalAnalysis(src, table);

    //PrintTokenStream(tokens, table);

    std::vector<AstNodeRef> ast = Parser::Parse(tokens);


    //AstPrinter().Print(std::cout, ast, table);
    if(!SemanticAnalyzer(ast, table).Analyze())
        return 1;

    std::string codegen = CCodegen(ast, table).Generate();
    //Print("%", codegen);
    
    std::string codegen_path = ToCCodegenFilepath(src_filepath);
    {
        std::ofstream codegen_file(codegen_path);
        assert(codegen_file.is_open());
        codegen_file << codegen;
    }

    return CCompiler::Compile(
        codegen_path
    );
}