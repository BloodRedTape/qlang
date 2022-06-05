#include "ast_printer.hpp"
#include "ast/ast.hpp"
#include "ast/statement.hpp"
#include "ast/function.hpp"

void AstPrinter::Print(std::ostream& stream, const Ast& ast, const SymbolTable& table){
	for(const AstNodeRef &node: ast){
		PrintAstNode(stream, node, table);
	}
}

void AstPrinter::PrintIndent(std::ostream& stream){
	for (int i = 0; i < m_Scope; i++)
		stream << "  ";
}

void AstPrinter::PrintAstNode(std::ostream& stream, const AstNodeRef& node, const SymbolTable& table){

	switch (node->Type) {
#if 0
	case AstNodeType::Statement:{
		AstStatement *stmt = (AstStatement *)node.get();
		stream << StatementTypeString(stmt->StmtType) << "Statement\n";
		//PrintStatement(stmt);
	}break;
#endif
	case AstNodeType::FunctionDeclaration: {
		AstFunctionDeclaration *func = (AstFunctionDeclaration*)node.get();
		stream << "FunctionDecl";
		if(func->Qualifiers == FunctionQualifier::Extern)
			stream << "<Extern>";
		stream << ": " << table[func->Identifier] << "\n"; 
		Indent(); 
		for (auto param : func->Parameters) {
			PrintIndent(stream);
			stream << "ParamDecl";
			stream << "<" << KeywordTypeString(param.DataType) << ">: ";
			stream << table[param.IdentifierIndex] << '\n'; 
		}
		PrintIndent(stream);
		stream << "ReturnDecl" << "<" << KeywordTypeString(func->ReturnType) << ">" << '\n';

		if(func->Body){
			PrintIndent(stream);
			PrintStatement(stream, &func->Body.value(), table);
		}
		UnIndent();
	}break;
	default:
		assert(false);
	}
}

void AstPrinter::PrintStatement(std::ostream& stream, const AstStatement *stmt, const SymbolTable& table){
	switch (stmt->StmtType) {
	case StatementType::Empty: 
		stream << "EmptyStatement\n";
		break;
	case StatementType::Compound:{
		stream << "CompoundStatement\n";
		CompoundStatement *compund = (CompoundStatement*)stmt;
		Indent();
		for (const auto &stmt : compund->Statements) {
			PrintIndent(stream);
			PrintStatement(stream, stmt.get(), table);
		}
		UnIndent();
	}break;
	case StatementType::Var:{
		stream << "VarStatement";
		VarStatement *var = (VarStatement*)stmt;
		stream << "<" << KeywordTypeString(var->DataType) << ">: " << table[var->IdentifierIndex] << '\n';
		if(var->InitialValue){
			Indent();
			PrintIndent(stream);
			PrintExpression(stream, var->InitialValue, table);
			UnIndent();
		}
	}break;
	default:
		assert(false);
	}
}

void AstPrinter::PrintExpression(std::ostream& stream, const ExpressionRef& expr_ptr, const SymbolTable& table){
	Expression *expr = expr_ptr.get();
	switch (expr->Type) {
	case ExpressionType::IntegerLiteral: {
		IntegerLiteralExpression *literal = (IntegerLiteralExpression*)expr;
		stream << "IntegerLiteral: $" << literal->Value << '\n';
	}break;
	case ExpressionType::Variable: {
		VariableExpression *var = (VariableExpression*)expr;
		stream << "Variable<>: " << table[var->IdentifierIndex]<< '\n';
	}break;
	default:
		assert(false);
	}
}
