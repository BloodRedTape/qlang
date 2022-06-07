#include "semantic.hpp"
#include "error.hpp"
#include <set>

SemanticAnalyzer::SemanticAnalyzer(const Ast& ast, const SymbolTable& table):
	m_Ast(ast),
	m_Table(table)
{}

bool SemanticAnalyzer::Analyze(){
	for (const AstNodeRef& node : m_Ast) {
		assert(node->Type == AstNodeType::FunctionDeclaration);

		if(!CheckFunction(*(AstFunctionDeclaration*)node.get()))
			return false;
	}
	return true;
}

bool SemanticAnalyzer::CheckFunction(const AstFunctionDeclaration& func){
	if(!func.Body)return true;
	
	m_Stack.Push({});

	for(auto param: func.Parameters)
		m_Stack[0].Declare(param.IdentifierIndex);
	bool res = CheckStatement(*func.Body);

	m_Stack.Pop();

	return res;
}

bool SemanticAnalyzer::CheckExpression(const Expression& expr){
	if (expr.Type == ExpressionType::IntegerLiteral)
		return true;

	if (expr.Type == ExpressionType::Variable){
		Token identifier = ((const VariableExpression&)expr).Identifier;
		if(!Find(identifier.IdentifierIndex))
			return Error("Semantics", "identifier '%'[line: %] is undefined", m_Table[identifier.IdentifierIndex], identifier.Line);
	}

	if (expr.Type == ExpressionType::UnaryOperator)
		return CheckExpression(*((const UnaryOperatorExpression&)expr).Expr);
	if (expr.Type == ExpressionType::BinaryOperator)
		return CheckExpression(*((const BinaryOperatorExpression&)expr).Left)
			&& CheckExpression(*((const BinaryOperatorExpression&)expr).Right);

	if (expr.Type == ExpressionType::Call){
		Token identifier = ((const CallExpression&)expr).Identifier;
		if(!IsFunctionDefined(identifier.IdentifierIndex))
			return Error("Semantics", "Function '%'[line: %] is not defined", m_Table[identifier.IdentifierIndex], identifier.Line);
		for (const ExpressionRef& arg: ((const CallExpression&)expr).Args)
			if(!CheckExpression(*arg))
				return false;
	}

	return true;
}


bool SemanticAnalyzer::CheckStatement(const AstStatement &stmt){
	switch (stmt.StmtType) {
	case StatementType::Empty: return true;
	case StatementType::Compound: {
		m_Stack.Push({});
		for (const AstStatementRef &stmt : ((const CompoundStatement&)stmt).Statements)
			if(!CheckStatement(*stmt))
				return false;
		m_Stack.Pop();
	}break;
	case StatementType::If:{
		return CheckStatement(((const IfStatement&)stmt).IfBody)
			&& CheckStatement(((const IfStatement&)stmt).ElseBody)
			&& CheckExpression(*((const IfStatement&)stmt).Condition);
	}break;
	case StatementType::Var: {
		m_Stack[0].Declare(((const VarStatement&)stmt).IdentifierIndex);
		if(((const VarStatement&)stmt).InitialValue)
			return CheckExpression(*((const VarStatement&)stmt).InitialValue);
	}break;
	case StatementType::Expression: {
		return CheckExpression(*((const ExpressionStatement&)stmt).Expr);
	}break;
	case StatementType::While: {
		return CheckExpression(*((const WhileStatement&)stmt).Condition)
			&& CheckStatement(((const WhileStatement&)stmt).Body);
	}break;
	case StatementType::Return: {
		return CheckExpression(*((const ReturnStatement&)stmt).Result);
	}break;
	default: assert(false);
	}
	return true;
}	

bool SemanticAnalyzer::IsFunctionDefined(TokenAtom identifier){
	for (const AstNodeRef& node : m_Ast) {
		assert(node->Type == AstNodeType::FunctionDeclaration);

		if(((AstFunctionDeclaration*)node.get())->Identifier == identifier)
			return true;
	}
	return false;
}

