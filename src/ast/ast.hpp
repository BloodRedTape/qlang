#pragma once

#include <memory>
#include <vector>

#include "lexer.hpp"

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
	FunctionDeclaration = 1,
	Statement			= 2
};

const char *AstNodeTypeString(AstNodeType type);

struct AstNode {
	AstNodeType Type = AstNodeType::None;

	AstNode() = default;

	virtual ~AstNode() = default;
};

using AstNodeRef = std::unique_ptr<AstNode>;

template<typename Type, typename ...ArgsType>
AstNodeRef AstNew(ArgsType&&...args) {
	return std::make_unique<Type>(std::forward<ArgsType>(args)...);
}

using Ast = std::vector<AstNodeRef>;
