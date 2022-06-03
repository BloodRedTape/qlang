#include "parser.hpp"

TokenStream::TokenStream(std::vector<Token> data):
	m_Data(data),
	m_Current(0)
{}

void TokenStream::Consume(u64 count){
	m_Current += count;
}

Token TokenStream::Peek(u64 offset) const{
	if(m_Current + offset >= m_Data.size())
		return Token::Regular(TokenType::None);
	return m_Data[m_Current + offset];
}

bool TokenStream::End() const{
	return m_Current >= m_Data.size();
}

const char* AstNodeTypeString(AstNodeType type){
	switch (type) {
	case AstNodeType::VariableDefinition: "VariableDefinition";
	case AstNodeType::FunctionDeclaration: "FunctionDeclaration";
	case AstNodeType::FunctionDefinition: "FunctionDefinition";
	default: return (assert(false), "Shit happens");
	}
}

u64 AstVariableDefinition::TryParse(const TokenStream& stream, u64 offset){
	Token type		 = stream.Peek(offset + 0);
	Token identifier = stream.Peek(offset + 1);

	if(!type.IsKeyword(KeywordType::Int) || !identifier.IsType(TokenType::Identifier))
		return 0;

	DataType = type.KeywordIndex;
	IdentifierIndex = identifier.IdentifierIndex;

	return 2;
}

u64 AstFunctionDeclaration::TryParse(const TokenStream& stream, u64 offset){
	const u64 initial_offset = offset;

	if (stream.Peek(offset).IsKeyword(KeywordType::Extern)) {
		Qualifiers = FunctionQualifier::Extern;
		offset += 1;
	}

	Token fn_keyword = stream.Peek(offset++);
	Token identifier = stream.Peek(offset++);
	
	if(!fn_keyword.IsKeyword(KeywordType::Fn)
	|| !identifier.IsType(TokenType::Identifier))
		return 0;

	Identifier = identifier.IdentifierIndex;

	if(!stream.Peek(offset++).IsType(TokenType::OpenParentheses))// (
		return 0;

	for (;;) {
		AstVariableDefinition param;
		u64 count = param.TryParse(stream, offset);

		offset += count;

		if (!count && stream.Peek(offset).IsType(TokenType::Comma)){
			offset += 1;
			continue;	
		}

		if(!count){
			break;
		}

		Parameters.push_back(param);
	}

	if(!stream.Peek(offset++).IsType(TokenType::CloseParentheses)
	|| !stream.Peek(offset++).IsType(TokenType::Colon))// ):
		return 0;

	Token ret = stream.Peek(offset++);

	if(!ret.IsDataType())
		return 0;

	ReturnType = ret.KeywordIndex;

	return offset - initial_offset;
}

#define TryParse(type, stream, ast) do{\
	type node;\
	u64 count = node.TryParse(stream, 0);\
	if (count) {\
		stream.Consume(count);\
		ast.push_back(AstNew<type>(node));\
		continue;\
	}\
}while(0)

std::vector<AstNodeRef> Parser::Parse(TokenStream tokens){
	std::vector<AstNodeRef> ast;

	for(int i = 0; !tokens.End(); i++){
		if (tokens.Peek().IsType(TokenType::Semicolon)) {
			tokens.Consume();
			continue;
		}
	
		TryParse(AstFunctionDeclaration, tokens, ast);
				
		tokens.Consume();
	}

	return ast;
}
