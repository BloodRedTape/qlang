#pragma once

#include "lexer.hpp"
#include <memory>
#include <vector>
#include <ostream>

class TokenStream {
private:
	std::vector<Token> m_Data;
	size_t m_Current;
public:
	TokenStream(std::vector<Token> data);

	void Consume(u64 count = 1);

	Token Peek(u64 offset = 0)const;

	bool End()const;
};

enum class AstNodeType {
	None				= 0,
	VariableDefinition,
	FunctionDeclaration,
	FunctionDefinition,	
};

const char *AstNodeTypeString(AstNodeType type);

struct AstNode {
	AstNodeType Type = AstNodeType::None;

	AstNode() = default;

	virtual ~AstNode() = default;
};

struct AstVariableDefinition: AstNode{
	KeywordType DataType;
	TokenAtom IdentifierIndex;

	AstVariableDefinition(){ Type = AstNodeType::VariableDefinition; }

	u64 TryParse(const TokenStream &stream, u64 offset);
};

namespace FunctionQualifier{
enum Type: u8{
	None   = 0x00,
	Extern = 0x01
};
}//namespace FunctionQualifiers::

struct AstFunctionDeclaration: AstNode{
	FunctionQualifier::Type Qualifiers = FunctionQualifier::None;
	TokenAtom Identifier = InvalidTokenAtom;

	std::vector<AstVariableDefinition> Parameters;
	KeywordType ReturnType;

	AstFunctionDeclaration(){ Type = AstNodeType::FunctionDeclaration; }

	u64 TryParse(const TokenStream &stream, u64 offset);
};

struct AstFunctionDefinition : AstFunctionDeclaration {
	AstFunctionDefinition(){ Type = AstNodeType::FunctionDefinition; }
};

using AstNodeRef = std::unique_ptr<AstNode>;

template<typename Type, typename ...ArgsType>
AstNodeRef AstNew(ArgsType&&...args) {
	return std::make_unique<Type>(std::forward<ArgsType>(args)...);
}



class Parser {
public:
	static std::vector<AstNodeRef> Parse(TokenStream tokens);
};
