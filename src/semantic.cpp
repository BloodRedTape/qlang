#include "semantic.hpp"
#include "error.hpp"
#include <set>

bool SemanticAnalyzer::Analyze(const Ast& ast, const SymbolTable &table){
	for (const AstNodeRef& node : ast) {
		assert(node->Type == AstNodeType::FunctionDeclaration);

		if(!CheckFunction(*(AstFunctionDeclaration*)node.get(), table))
			return false;
	}
	return true;
}

class Scope {
private:
	std::set<TokenAtom> m_Identifiers;
public:
	bool Has(TokenAtom identifier)const {
		return m_Identifiers.find(identifier) != m_Identifiers.end();
	}

	void Declare(TokenAtom identifier) {
		m_Identifiers.insert(identifier);
	}
};

struct Stack {
	std::vector<Scope> Scopes;
	TokenAtom FunctionIdentifier = InvalidTokenAtom;
	
	bool Find(TokenAtom identifier)const{
		for (const Scope& scope : Scopes) {
			if(scope.Has(identifier))
				return true;
		}
		return false;
	}

	void Push(){ Scopes.emplace_back(); }

	void Pop(){ Scopes.pop_back(); }

	Scope &Top(){ return Scopes.back(); }
};

bool CheckExpression(const Stack& stack, const Expression& expr, const SymbolTable &table) {
	if(expr.Type == ExpressionType::IntegerLiteral)
		return true;

	if (expr.Type == ExpressionType::Variable){
		Token identifier = ((const VariableExpression&)expr).Identifier;
		if(!stack.Find(identifier.IdentifierIndex))
			return Error("Semantics", "In Function '%' identifier '%'[line: %] is undefined", table[stack.FunctionIdentifier], table[identifier.IdentifierIndex], identifier.Line);
	}

	if (expr.Type == ExpressionType::UnaryOperator)
		return CheckExpression(stack, *((const UnaryOperatorExpression&)expr).Expr, table);
	if (expr.Type == ExpressionType::BinaryOperator)
		return CheckExpression(stack, *((const BinaryOperatorExpression&)expr).Left, table)
			&& CheckExpression(stack, *((const BinaryOperatorExpression&)expr).Right, table);

	if (expr.Type == ExpressionType::Call)
		for (const ExpressionRef& arg: ((const CallExpression&)expr).Args)
			if(!CheckExpression(stack, *arg, table))
				return false;

	return true;
}

bool CheckStatement(Stack& stack, const AstStatement& stmt, const SymbolTable &table) {
	switch (stmt.StmtType) {
	case StatementType::Empty: return true;
	case StatementType::Compound: {
		stack.Push();
		for (const AstStatementRef &stmt : ((const CompoundStatement&)stmt).Statements)
			if(!CheckStatement(stack, *stmt, table))
				return false;
		stack.Pop();
	}break;
	case StatementType::If:{
		return CheckStatement(stack, ((const IfStatement&)stmt).IfBody, table)
			&& CheckStatement(stack, ((const IfStatement&)stmt).ElseBody, table)
			&& CheckExpression(stack, *((const IfStatement&)stmt).Condition, table);
	}break;
	case StatementType::Var: {
		stack.Top().Declare(((const VarStatement&)stmt).IdentifierIndex);
		if(((const VarStatement&)stmt).InitialValue)
			return CheckExpression(stack, *((const VarStatement&)stmt).InitialValue, table);
	}break;
	case StatementType::Expression: {
		return CheckExpression(stack, *((const ExpressionStatement&)stmt).Expr, table);
	}break;
	case StatementType::While: {
		return CheckExpression(stack, *((const WhileStatement&)stmt).Condition, table)
			&& CheckStatement(stack, ((const WhileStatement&)stmt).Body, table);
	}break;
	case StatementType::Return: {
		return CheckExpression(stack, *((const ReturnStatement&)stmt).Result, table);
	}break;
	default: assert(false);
	}

	return true;
}

bool SemanticAnalyzer::CheckFunction(const AstFunctionDeclaration& func, const SymbolTable &table){
	if(!func.Body)return true;

	Stack stack;
	stack.FunctionIdentifier = func.Identifier;
	stack.Push();
	for(auto param: func.Parameters)
		stack.Top().Declare(param.IdentifierIndex);

	return CheckStatement(stack, *func.Body, table);
}

