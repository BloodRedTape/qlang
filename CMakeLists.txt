cmake_minimum_required(VERSION 3.10)

project(qlang)

add_executable(qlang
	"./src/main.cpp"
	"./src/lexer.cpp"
	"./src/parser.cpp"
	"./src/ast/ast.cpp"
	"./src/ast/function.cpp"
	"./src/ast/statement.cpp"
	"./src/ast/expression.cpp"
 "src/ast_printer.hpp" "src/ast_printer.cpp")

target_compile_features(qlang PUBLIC cxx_std_17)

target_include_directories(qlang
	PRIVATE "./src"
)