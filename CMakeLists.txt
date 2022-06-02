cmake_minimum_required(VERSION 3.10)

project(qlang)

add_executable(qlang
	"./src/main.cpp"
 "src/lexer.hpp" "src/lexer.cpp" "src/utils.hpp")