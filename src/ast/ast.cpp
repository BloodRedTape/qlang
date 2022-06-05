#include "ast/ast.hpp"
#include <assert.h>

TokenStream::TokenStream(std::vector<Token> data):
	m_Data(data),
	m_Current(0)
{}

void TokenStream::Consume(u64 count){
	m_Current += count;
}

Token TokenStream::Peek(u64 offset) const{
	assert(!End());
	if(m_Current + offset >= m_Data.size())
		return Token::Regular(TokenType::None, 0);
	return m_Data[m_Current + offset];
}

bool TokenStream::End() const{
	return m_Current >= m_Data.size();
}

const char* AstNodeTypeString(AstNodeType type){
	switch (type) {
	case AstNodeType::FunctionDeclaration: "FunctionDeclaration";
	case AstNodeType::Statement: "Statement";
	default: return (assert(false), "Shit happens");
	}
}
