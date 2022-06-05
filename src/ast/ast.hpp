#pragma once

#include <memory>
#include <vector>

#include "lexer.hpp"

constexpr u64 InvalidCount = -1;

class TokenStream {
private:
	std::vector<Token> m_Data;
	size_t m_Current;
public:
	TokenStream(std::vector<Token> data);

	void Consume(u64 count = 1);

	Token Peek(u64 offset = 0)const;

	bool End()const;

	bool HasRepeated(TokenType type, u64 count, u64 start)const{
		for(u64 i = 0; i<count; i++)
			if(!Peek(start + i).IsType(type))
				return false;
		return true;
	}

	u64 CountUntilFirst(TokenType type, u64 start) {
		for(u64 count = 0;!End();count++) {
			if(Peek(start + count).IsType(type))
				return count;
		}
		return InvalidCount;
	}
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
