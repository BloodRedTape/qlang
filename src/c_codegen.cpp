#include "c_codegen.hpp"
#include "error.hpp"

CCodegen::CCodegen(const Ast& ast, const SymbolTable& table):
	m_Ast(ast),
	m_Table(table)
{}

std::string CCodegen::Generate(){
	std::string result;
	result += HeaderBoilderplate();

	for (const AstNodeRef& node : m_Ast)
		result += GenerateFunctionDeclaration((const AstFunctionDeclaration&)*node);

	for (const AstNodeRef& node : m_Ast)
		if(((const AstFunctionDeclaration&)*node).Body)
			result += GenerateFunctionDefinition((const AstFunctionDeclaration&)*node);

	
	result += GenerateEntryPoint();

	return result;
}

std::string CCodegen::HeaderBoilderplate() const{
	return R"(
#include <cstdint>
#include <cstdio>

using s64 = std::int64_t;

void Println(s64 num){
	printf("%lld\n", num);	
}

)";
}

const char* ToCTypeSpecifier(KeywordType keyword) {
	switch (keyword) {
	case KeywordType::Void:	return "void";
	case KeywordType::Int:	return "s64";
	default: return (assert(false), "Shit happens");
	}
}

std::string CCodegen::GenerateFunctionHeader(const AstFunctionDeclaration& decl){
	std::stringstream result;
	result << ToCTypeSpecifier(decl.ReturnType) << ' ' << m_Table[decl.Identifier];
	result << '(';
	for(size_t i = 0; decl.Parameters.size(); i++){
		FunctionParamDecl param = decl.Parameters[i];
		result << ToCTypeSpecifier(param.DataType) << ' ' << m_Table[param.IdentifierIndex];
		if(i + 1 == decl.Parameters.size())
			break;
		result << ", ";
	}
	result << ")";
	return result.str();
}

std::string CCodegen::GenerateFunctionDeclaration(const AstFunctionDeclaration& decl){
	return GenerateFunctionHeader(decl) + ";\n";
}

std::string CCodegen::GenerateFunctionDefinition(const AstFunctionDeclaration& decl){
	return GenerateFunctionHeader(decl) + GenerateStatement(*decl.Body);
}

std::string CCodegen::GenerateStatement(const AstStatement& stmt){
	switch (stmt.StmtType) {
	case StatementType::Empty:
		return {};
	case StatementType::Compound: {
		std::stringstream result;
		result << "{\n";
		for(const AstStatementRef &stmt_ref: ((const CompoundStatement&)stmt).Statements)
			result << GenerateStatement(*stmt_ref);
		result << "}\n";
		return result.str();
	}
	case StatementType::If: {
		const IfStatement &if_stmt = (const IfStatement &)stmt;
		std::stringstream result;
		result << "if(" + GenerateExpression(*if_stmt.Condition) << ")\n" << GenerateStatement(if_stmt.IfBody);
		if(if_stmt.ElseBody)
			result << "else\n" << GenerateStatement(if_stmt.ElseBody);

		return result.str();
	}
	case StatementType::Var: {
		const VarStatement &var = (const VarStatement &)stmt;
		std::stringstream result;
		result << ToCTypeSpecifier(var.DataType) << " " << m_Table[var.IdentifierIndex];
		if(var.InitialValue)
			result << " = " << GenerateExpression(*var.InitialValue);
		result << ";\n";
		return result.str();
	}
	case StatementType::Expression: {
		return GenerateExpression(*((const ExpressionStatement &)stmt).Expr) + ";\n";
	}
	case StatementType::While: {
		const WhileStatement &loop = (const WhileStatement &)stmt;
		std::stringstream result;
		result << "while(" << GenerateExpression(*loop.Condition) << ")\n";
		result << GenerateStatement(loop.Body);
		return result.str();
	}
	case StatementType::Return: {
		return "return " + GenerateExpression(*((const ReturnStatement &)stmt).Result) + ";\n";
	}
	default: return (assert(false), "Shit happens");
	}
}

std::string CCodegen::GenerateExpression(const Expression& expr){
	switch (expr.Type) {
	case ExpressionType::IntegerLiteral: {
		const auto &lit = (const IntegerLiteralExpression&)expr;
		return std::to_string((s64)lit.Value);
	}
	case ExpressionType::Variable: {
		const auto &var = (const VariableExpression &)expr;
		return m_Table[var.Identifier.IdentifierIndex];
	}
	case ExpressionType::UnaryOperator: {
		const auto &unary = (const UnaryOperatorExpression&)expr;
		std::string inside = "(" + GenerateExpression(*unary.Expr) + ")";
		
		switch (unary.UnaryType) {
		case UnaryOperatorType::Negative: return "(-" + inside + ")";
		case UnaryOperatorType::PreIncrement:return "(++" + inside + ")";
		case UnaryOperatorType::PreDecrement:return "(--" + inside + ")";
		case UnaryOperatorType::PostIncrement:return "(" + inside + "++)";
		case UnaryOperatorType::PostDecrement:return "(" + inside + "--)";
		default: return (assert(false), "Shit happens");
		}
	}
	case ExpressionType::BinaryOperator: {
		const auto &binary = (const BinaryOperatorExpression &)expr;

		return GenerateExpression(*binary.Left) + BinaryOperatorTokenString(binary.OpType) + GenerateExpression(*binary.Right);
	}
	case ExpressionType::Call: {
		const auto &call = (const CallExpression &)expr;
		std::stringstream result;
		result << m_Table[call.Identifier.IdentifierIndex] << "(";
		for(size_t i = 0; call.Args.size(); i++){
			result << GenerateExpression(*call.Args[i]);
			if(i + 1 == call.Args.size())
				break;
			result << ",";
		}
		result << ")";
		return result.str();
	}
	default: return (assert(false), "Shit happens");
	}
}

std::string CCodegen::GenerateEntryPoint() const{
	return R"(
int main(int argc, char **argv){
	return Main();
}
)";
}
